#include "minigame_redlightgreenlight.h"

static bool rlgl_newGame = true;
static uint16_t rlgl_timer = 0;
static uint16_t rlgl_stateTimer = 0;
static bool rlgl_isGoMode = true;
static int8_t rlgl_playerX = 0;
static uint16_t rlgl_stateDuration = 0;
static uint16_t rlgl_stopTime = 0;

static bool rlgl_failedState = false;
static uint16_t rlgl_failedTime = 0;

static uint16_t rlgl_transitionDisplayTime = 0;
static bool rlgl_showTransitionText = false;

void doRedLightGreenLightGame() {
  uint16_t currentMillis = (uint16_t)millis();

  if (rlgl_failedState) {
    arduboy.setCursor(50, 30);
    arduboy.print(F("FAIL!"));
    if ((uint16_t)(currentMillis - rlgl_failedTime) >= 2000) {
      rlgl_failedState = false;
      rlgl_newGame = true;
      gameState = STATE_INTERMISSION;
    }
    return;
  }

  if (rlgl_newGame) {
    rlgl_timer = currentMillis;
    rlgl_stateTimer = currentMillis;
    rlgl_isGoMode = true;
    rlgl_playerX = 5;
    rlgl_stateDuration = random(500, 801); // Short first GO → early first stop
    rlgl_showTransitionText = true;
    rlgl_transitionDisplayTime = currentMillis;
    sound.tone(NOTE_C5, 200);
    turnOnLED(COLOR_GREEN, rlgl_stateDuration + 250);
    rlgl_newGame = false;
  }

  // Handle State Toggling
  if ((uint16_t)(currentMillis - rlgl_stateTimer) >= rlgl_stateDuration) {
    rlgl_isGoMode = !rlgl_isGoMode;
    rlgl_stateTimer = currentMillis;
    rlgl_stateDuration =
        rlgl_isGoMode ? random(2000, 2501)  // GO: 2.0-2.5s window
                      : random(1000, 2251); // STOP: 1.0-2.25s, unpredictable
    rlgl_showTransitionText = true;
    rlgl_transitionDisplayTime = currentMillis;

    if (rlgl_isGoMode) {
      sound.tone(NOTE_C5, 200);
      turnOnLED(COLOR_GREEN, rlgl_stateDuration + 250);
    } else {
      sound.tone(NOTE_C4, 200);
      turnOnLED(COLOR_RED, rlgl_stateDuration + 250);
      rlgl_stopTime = currentMillis;
    }
  }

  // Draw Finish Line
  int8_t finishLineX = 115;
  arduboy.drawFastVLine(finishLineX, 0, MAX_Y_POS, WHITE);
  arduboy.drawFastVLine(finishLineX + 2, 0, MAX_Y_POS, WHITE);

  // Draw Player
  arduboy.fillRect(rlgl_playerX, MAX_Y_POS / 2 - 4, 8, 8, WHITE);

  // Draw Transition Text
  if (rlgl_showTransitionText) {
    if ((uint16_t)(currentMillis - rlgl_transitionDisplayTime) < 1000) {
      arduboy.setCursor(50, 15);
      if (rlgl_isGoMode) {
        arduboy.print(F("GO!"));
      } else {
        arduboy.print(F("STOP!"));
      }
    } else {
      rlgl_showTransitionText = false;
    }
  }

  // Handle Input and Movement
  bool isMoving = arduboy.pressed(A_BUTTON) || arduboy.pressed(B_BUTTON);

  if (isMoving) {
    // Move 5 out of every 8 frames = 0.625px/frame (~25% faster than original)
    if (arduboy.frameCount % 8 < 5) {
      rlgl_playerX += 1;
    }

    // Check Failure Condition
    if (!rlgl_isGoMode && ((uint16_t)(currentMillis - rlgl_stopTime) > 250)) {
      turnOffLED();
      int pointsEarned = (rlgl_playerX * 15) >> 4;
      if (pointsEarned > 100)
        pointsEarned = 100;
      int rem = pointsEarned % 5;
      if (rem >= 3)
        pointsEarned += (5 - rem);
      else
        pointsEarned -= rem;
      addScore(pointsEarned);

      turnOnLED(COLOR_RED, 1000);
      rlgl_failedState = true;
      rlgl_failedTime = currentMillis;
      return;
    }
  }

  // Win Condition
  if (rlgl_playerX + 8 >= finishLineX) {
    addScore(100);
    turnOnLED(COLOR_GREEN, 1000);
    turnOffLED();
    rlgl_newGame = true;
    gameState = STATE_INTERMISSION;
    return;
  }

  // Time Limit Condition (e.g. 12 seconds max overall context)
  if ((uint16_t)(currentMillis - rlgl_timer) >= 12000) {
    int pointsEarned = (rlgl_playerX * 15) >> 4;
    if (pointsEarned > 100)
      pointsEarned = 100;
    int rem = pointsEarned % 5;
    if (rem >= 3)
      pointsEarned += (5 - rem);
    else
      pointsEarned -= rem;
    addScore(pointsEarned);

    turnOffLED();
    rlgl_newGame = true;
    gameState = STATE_INTERMISSION;
    return;
  }
}
