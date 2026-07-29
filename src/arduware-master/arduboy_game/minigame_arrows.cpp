#include "minigame_arrows.h"

// Bring in the sprites we need from the main file for now, or declare them
// extern if they are in another header
extern const uint8_t PROGMEM right_arrow[];
extern const uint8_t PROGMEM left_arrow[];
extern const uint8_t PROGMEM down_arrow[];
extern const uint8_t PROGMEM up_arrow[];
extern const uint8_t PROGMEM right_arrow_hollow[];
extern const uint8_t PROGMEM left_arrow_hollow[];
extern const uint8_t PROGMEM down_arrow_hollow[];
extern const uint8_t PROGMEM up_arrow_hollow[];
extern const uint8_t PROGMEM a_button[];
extern const uint8_t PROGMEM b_button[];
extern const uint8_t PROGMEM button_filled[];

uint16_t arrowsGame_lastSpawnTime = 0;
uint16_t arrowsGame_nextSpawnDelay = 1000;
// number of arrows per game
uint8_t arrowsGame_numArrows = 10;
uint8_t arrowsGame_spawnTotal = 0;
int16_t arrowsGame_targetY = 48;
// x locations for the hit targets
int16_t arrowsGame_leftX = 5;
int16_t arrowsGame_rightX = 25;
int16_t arrowsGame_upX = 45;
int16_t arrowsGame_downX = 65;
int16_t arrowsGame_bX = 105;
int16_t arrowsGame_aX = 85;

#define MAX_ACTIVE_ARROWS 4
struct FallingArrow {
  bool active;
  int16_t x;
  float y;
  float speed;
  uint8_t target;
};

FallingArrow arrowsGame_arrows[MAX_ACTIVE_ARROWS];
// x,y,frame (4 frames)
int16_t arrowsGame_XPos[] = {-1, -1, 0};

void arrowsGameAnimateSprites() {
  // Animation logic for missed hits removed to save space
}

void arrowsGameSpawn(int16_t x, uint8_t target) {
  for (int i = 0; i < MAX_ACTIVE_ARROWS; i++) {
    if (!arrowsGame_arrows[i].active) {
      arrowsGame_arrows[i].active = true;
      arrowsGame_arrows[i].x = x;
      arrowsGame_arrows[i].y = 0;
      arrowsGame_arrows[i].target = target;
      arrowsGame_arrows[i].speed = random(12, 16) / 10.0; // 1.2 to 1.5 speed
      arrowsGame_spawnTotal++;
      break;
    }
  }
}
void arrowsGameDespawn(int i) { arrowsGame_arrows[i].active = false; }
void arrowsGameSpawnX(int i) {
  arrowsGame_XPos[0] = arrowsGame_arrows[i].x;
  arrowsGame_XPos[1] = (int16_t)arrowsGame_arrows[i].y;
  arrowsGame_XPos[2] = 0;
}
void arrowsGameHit(int i) {
  addScore(10);
  arrowsGameDespawn(i);
}

void doArrowsGame() {
  // Check if we finished
  bool anyActive = false;
  for (int i = 0; i < MAX_ACTIVE_ARROWS; i++) {
    if (arrowsGame_arrows[i].active)
      anyActive = true;
  }

  if (arrowsGame_spawnTotal >= arrowsGame_numArrows && !anyActive &&
      arrowsGame_XPos[0] == -1) {
    for (int i = 0; i < MAX_ACTIVE_ARROWS; i++)
      arrowsGameDespawn(i);
    arrowsGame_spawnTotal = 0;
    arrowsGame_lastSpawnTime = 0;
    gameState = STATE_INTERMISSION;
    return;
  }

  // Spawning logic
  if (arrowsGame_spawnTotal < arrowsGame_numArrows) {
    if (arrowsGame_lastSpawnTime == 0) {
      arrowsGame_lastSpawnTime = (uint16_t)millis();
      arrowsGame_nextSpawnDelay = random(400, 700);
    } else {
      // Spawn occasionally based on the randomized target delay
      if ((uint16_t)((uint16_t)millis() - arrowsGame_lastSpawnTime) >
          arrowsGame_nextSpawnDelay) {
        arrowsGame_lastSpawnTime = (uint16_t)millis();
        arrowsGame_nextSpawnDelay = random(400, 700);
        uint8_t target = random(6);
        switch (target) {
        case 0:
          arrowsGameSpawn(arrowsGame_leftX, target);
          break;
        case 1:
          arrowsGameSpawn(arrowsGame_rightX, target);
          break;
        case 2:
          arrowsGameSpawn(arrowsGame_upX, target);
          break;
        case 3:
          arrowsGameSpawn(arrowsGame_downX, target);
          break;
        case 4:
          arrowsGameSpawn(arrowsGame_bX, target);
          break;
        case 5:
          arrowsGameSpawn(arrowsGame_aX, target);
          break;
        }
      }
    }
  }

  // draw the timing target buttons
  Sprites::drawExternalMask(arrowsGame_leftX, arrowsGame_targetY,
                            left_arrow_hollow, &left_arrow[2], 0, 0);
  Sprites::drawExternalMask(arrowsGame_rightX, arrowsGame_targetY,
                            right_arrow_hollow, &right_arrow[2], 0, 0);
  Sprites::drawExternalMask(arrowsGame_upX, arrowsGame_targetY, up_arrow_hollow,
                            &up_arrow[2], 0, 0);
  Sprites::drawExternalMask(arrowsGame_downX, arrowsGame_targetY,
                            down_arrow_hollow, &down_arrow[2], 0, 0);
  Sprites::drawExternalMask(arrowsGame_bX, arrowsGame_targetY, b_button,
                            &button_filled[2], 0, 0);
  Sprites::drawExternalMask(arrowsGame_aX, arrowsGame_targetY, a_button,
                            &button_filled[2], 0, 0);

  // update and draw falling arrows
  for (int i = 0; i < MAX_ACTIVE_ARROWS; i++) {
    if (arrowsGame_arrows[i].active) {
      const uint8_t *spawnBmp;
      switch (arrowsGame_arrows[i].target) {
      case 0:
        spawnBmp = left_arrow;
        break;
      case 1:
        spawnBmp = right_arrow;
        break;
      case 2:
        spawnBmp = up_arrow;
        break;
      case 3:
        spawnBmp = down_arrow;
        break;
      case 4:
        spawnBmp = b_button;
        break;
      case 5:
        spawnBmp = a_button;
        break;
      default:
        spawnBmp = left_arrow;
        break;
      }

      Sprites::drawExternalMask(arrowsGame_arrows[i].x,
                                (int16_t)arrowsGame_arrows[i].y, spawnBmp,
                                spawnBmp + 2, 0, 0);

      arrowsGame_arrows[i].y += arrowsGame_arrows[i].speed;

      if (arrowsGame_arrows[i].y >= MAX_Y_POS) {
        arrowsGameDespawn(i);
      }
    }
  }

  // light up the button presses, do collision detection
  auto handleButtonPress = [](uint8_t buttonId, int16_t buttonX,
                              const uint8_t *activeSprite,
                              const uint8_t *activeSpriteMask) {
    Sprites::drawExternalMask(buttonX, arrowsGame_targetY, activeSprite,
                              activeSpriteMask, 0, 0);
    bool hit = false;
    for (int i = 0; i < MAX_ACTIVE_ARROWS; i++) {
      if (arrowsGame_arrows[i].active &&
          arrowsGame_arrows[i].target == buttonId) {
        if (abs((int16_t)arrowsGame_arrows[i].y - arrowsGame_targetY) <= 8) {
          turnOnLED(COLOR_GREEN);
          arrowsGameHit(i);
          hit = true;
          break; // only hit one per press
        }
      }
    }
    if (!hit) {
      turnOnLED(COLOR_RED);
      addScore(-5);
    }
  };

  if (arduboy.justPressed(LEFT_BUTTON)) {
    handleButtonPress(0, arrowsGame_leftX, left_arrow, &left_arrow[2]);
  } else if (arduboy.justPressed(RIGHT_BUTTON)) {
    handleButtonPress(1, arrowsGame_rightX, right_arrow, &right_arrow[2]);
  } else if (arduboy.justPressed(UP_BUTTON)) {
    handleButtonPress(2, arrowsGame_upX, up_arrow, &up_arrow[2]);
  } else if (arduboy.justPressed(DOWN_BUTTON)) {
    handleButtonPress(3, arrowsGame_downX, down_arrow, &down_arrow[2]);
  } else if (arduboy.justPressed(B_BUTTON)) {
    handleButtonPress(4, arrowsGame_bX, button_filled, &button_filled[2]);
  } else if (arduboy.justPressed(A_BUTTON)) {
    handleButtonPress(5, arrowsGame_aX, button_filled, &button_filled[2]);
  }
  arrowsGameAnimateSprites();
}
