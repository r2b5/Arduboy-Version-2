#include "minigame_hurdles.h"

// Game logic state variables
static bool hurdles_newGame = true;
static uint16_t hurdles_gameStartTimer = 0;
static uint16_t hurdles_lastScoreTimer = 0;

// Player states
static int8_t hurdles_playerY = 0;
static bool hurdles_isJumping = false;
static bool hurdles_isDucking = false;
static uint16_t hurdles_jumpTimer = 0;
static uint16_t hurdles_duckTimer = 0;

#define MAX_HURDLES 3
static uint8_t hurdles_obstacleType[MAX_HURDLES] = {0};
static int8_t hurdles_obstacleX[MAX_HURDLES] = {0};

static int8_t hurdles_dotX[3];
static int8_t hurdles_dotY[3];

static bool hurdles_failedState = false;
static uint16_t hurdles_failedTime = 0;

// Constants for bounding box layout
const int8_t playerX = 20;
const int8_t groundY = 50;
const int8_t playerNormalW = 6;
const int8_t playerNormalH = 12;

void doHurdlesGame() {
  uint16_t currentMillis = (uint16_t)millis();

  if (hurdles_newGame) {
    hurdles_gameStartTimer = currentMillis;
    hurdles_lastScoreTimer = currentMillis;
    hurdles_isJumping = false;
    hurdles_isDucking = false;
    hurdles_playerY = groundY - playerNormalH;
    for (uint8_t i = 0; i < MAX_HURDLES; i++) {
      hurdles_obstacleType[i] = 0;
      hurdles_obstacleX[i] = 128;
    }
    hurdles_failedState = false;
    hurdles_newGame = false;
    for (uint8_t i = 0; i < 3; i++) {
      hurdles_dotX[i] = random(0, 128);
      hurdles_dotY[i] = random(10, groundY - 5);
    }
    return;
  }

  // FAIL SCREEN STATE
  if (hurdles_failedState) {
    arduboy.setCursor(48, 28);
    arduboy.print(F("FAIL!"));

    if ((uint16_t)(currentMillis - hurdles_failedTime) > 2000) {
      hurdles_newGame = true;
      gameState = STATE_INTERMISSION;
    }
    return;
  }

  // UPDATE LOGIC: Movement processing
  // Process ducking timeout (250ms duration)
  if (hurdles_isDucking &&
      ((uint16_t)(currentMillis - hurdles_duckTimer) >= 250)) {
    hurdles_isDucking = false;
    hurdles_playerY = groundY - playerNormalH;
  }

  // Process jump timeout (250ms duration)
  if (hurdles_isJumping &&
      ((uint16_t)(currentMillis - hurdles_jumpTimer) >= 250)) {
    hurdles_isJumping = false;
    hurdles_playerY = groundY - playerNormalH;
  }

  // Input Handling
  if (!hurdles_isJumping && !hurdles_isDucking) {
    if (arduboy.justPressed(UP_BUTTON) || arduboy.justPressed(A_BUTTON)) {
      hurdles_isJumping = true;
      hurdles_jumpTimer = currentMillis;
      hurdles_playerY = groundY - playerNormalH - 18; // Jump height
    } else if (arduboy.justPressed(DOWN_BUTTON) ||
               arduboy.justPressed(B_BUTTON)) {
      hurdles_isDucking = true;
      hurdles_duckTimer = currentMillis;
      hurdles_playerY = groundY - (playerNormalH / 2); // Duck height
    }
  }

  // Process Scenery Movement (shift left every frame)
  // Shift background dots
  for (uint8_t i = 0; i < 3; i++) {
    hurdles_dotX[i] -= 2; // Slower than foreground
    if (hurdles_dotX[i] < 0) {
      hurdles_dotX[i] = 128;
      hurdles_dotY[i] = random(10, groundY - 5);
    }
  }

  // Process Scenery Movement first
  for (uint8_t i = 0; i < MAX_HURDLES; i++) {
    if (hurdles_obstacleType[i] != 0) {
      // 4x relative terrain speed
      hurdles_obstacleX[i] -= 4;

      // Despawn
      if (hurdles_obstacleX[i] < -15) {
        hurdles_obstacleType[i] = 0;
      }
    }
  }

  // Then Process Spawning
  // 1. Check if the rightmost active obstacle is far enough away
  bool canSpawn = true;
  for (uint8_t j = 0; j < MAX_HURDLES; j++) {
    if (hurdles_obstacleType[j] != 0 && hurdles_obstacleX[j] > 80) {
      // An obstacle was just spawned recently, hold off
      canSpawn = false;
      break;
    }
  }

  // 2. Wait random interval to spawn next
  if (canSpawn && random(0, 100) < 12) {
    // Find the first empty array slot
    for (uint8_t i = 0; i < MAX_HURDLES; i++) {
      if (hurdles_obstacleType[i] == 0) {
        hurdles_obstacleX[i] = 128;
        hurdles_obstacleType[i] = random(1, 3); // 1 = hurdle, 2 = ceiling
        break; // Only spawn one obstacle per frame
      }
    }
  }

  // COLLISION DETECTION
  int8_t playerW = playerNormalW;
  int8_t playerH = hurdles_isDucking ? (playerNormalH / 2) : playerNormalH;

  for (uint8_t i = 0; i < MAX_HURDLES; i++) {
    if (hurdles_obstacleType[i] != 0) {
      int8_t obsX = hurdles_obstacleX[i];
      int8_t obsY = (hurdles_obstacleType[i] == 1) ? groundY - 8 : groundY - 24;
      int8_t obsW = 8;
      int8_t obsH = (hurdles_obstacleType[i] == 1) ? 8 : 16;

      // AABB Bounds Check
      if (playerX < obsX + obsW && playerX + playerW > obsX &&
          hurdles_playerY < obsY + obsH && hurdles_playerY + playerH > obsY) {
        // HIT!
        turnOnLED(COLOR_RED, 1000);
        sound.tone(NOTE_C4, 500);
        hurdles_failedState = true;
        hurdles_failedTime = currentMillis;
        return;
      }
    }
  }

  // SCORE LOGIC (Award point every 2s)
  if ((uint16_t)(currentMillis - hurdles_lastScoreTimer) >= 2000) {
    hurdles_lastScoreTimer += 2000;
    addScore(10);
    turnOnLED(COLOR_GREEN, 100);
  }

  // Turn off LED after flash
  if ((uint16_t)(currentMillis - hurdles_lastScoreTimer) > 100) {
    turnOffLED();
  }

  // WIN LOGIC (End after 12s total)
  if ((uint16_t)(currentMillis - hurdles_gameStartTimer) >= 12100) {
    turnOffLED();
    hurdles_newGame = true;
    gameState = STATE_INTERMISSION;
    return;
  }

  // RENDER PHASE

  // Draw ground
  arduboy.drawLine(0, groundY, 128, groundY, WHITE);

  // Draw background dots
  for (uint8_t i = 0; i < 3; i++) {
    arduboy.drawPixel(hurdles_dotX[i], hurdles_dotY[i], WHITE);
  }

  // Draw player
  arduboy.fillRect(playerX, hurdles_playerY, playerW, playerH, WHITE);

  // Draw obstacle
  for (uint8_t i = 0; i < MAX_HURDLES; i++) {
    if (hurdles_obstacleType[i] != 0) {
      if (hurdles_obstacleType[i] == 1) {
        int8_t obsY = groundY - 8;
        arduboy.fillRect(hurdles_obstacleX[i], obsY, 8, 8, WHITE);
      } else {
        // Ceiling obstacle: 16px height, positioned so jumping hits it
        arduboy.fillRect(hurdles_obstacleX[i], groundY - 24, 8, 16, WHITE);
      }
    }
  }
}
