#include "minigame_simon.h"

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

uint16_t simonGame_patternCount = 5;
uint16_t simonGame_upPos[] = {40, 20};
uint16_t simonGame_downPos[] = {40, 40};
uint16_t simonGame_leftPos[] = {30, 30};
uint16_t simonGame_rightPos[] = {50, 30};
uint16_t simonGame_APos[] = {70, 30};
uint16_t simonGame_BPos[] = {90, 30};
uint16_t simonGame_pattern[5];
bool simonGame_newGame = true;
int simonGame_timer = 0;
uint16_t simonGame_prev = 6;
uint16_t simonGame_playerIdx = 0;
uint16_t simonGame_playTimeSeconds = 10;
bool simonGame_failedState = false;
int simonGame_failedTime = 0;

void simonGameLightUp(uint16_t idx) {
  switch (idx) {
  case 0:
    Sprites::drawExternalMask(simonGame_leftPos[0], simonGame_leftPos[1],
                              left_arrow, &left_arrow[2], 0, 0);
    break;
  case 1:
    Sprites::drawExternalMask(simonGame_rightPos[0], simonGame_rightPos[1],
                              right_arrow, &right_arrow[2], 0, 0);
    break;
  case 2:
    Sprites::drawExternalMask(simonGame_upPos[0], simonGame_upPos[1], up_arrow,
                              &up_arrow[2], 0, 0);
    break;
  case 3:
    Sprites::drawExternalMask(simonGame_downPos[0], simonGame_downPos[1],
                              down_arrow, &down_arrow[2], 0, 0);
    break;
  case 4:
    Sprites::drawExternalMask(simonGame_APos[0], simonGame_APos[1],
                              button_filled, &button_filled[2], 0, 0);
    break;
  case 5:
    Sprites::drawExternalMask(simonGame_BPos[0], simonGame_BPos[1],
                              button_filled, &button_filled[2], 0, 0);
    break;
  default:
    break;
  }
}

void doSimonGame() {
  if (simonGame_failedState) {
    arduboy.setCursor(50, 30);
    arduboy.print(F("FAIL!"));
    if (millis() / 1000 - simonGame_failedTime >= 2) {
      simonGame_failedState = false;
      simonGame_newGame = true;
      gameState = STATE_INTERMISSION;
    }
    return;
  }

  uint16_t pattern_idx;
  uint16_t seconds_elapsed;
  uint16_t cur;
  // new game, generate the pattern
  if (simonGame_newGame) {
    for (int i = 0; i < simonGame_patternCount; i++) {
      cur = random(6);
      while (cur == simonGame_prev) {
        cur = random(6);
      }
      simonGame_pattern[i] = cur;
      simonGame_prev = cur;
    }
    simonGame_newGame = false;
    simonGame_timer = millis() / 1000;
    simonGame_playerIdx = 0;
  }
  // draw the buttons
  Sprites::drawExternalMask(simonGame_leftPos[0], simonGame_leftPos[1],
                            left_arrow_hollow, &left_arrow[2], 0, 0);
  Sprites::drawExternalMask(simonGame_rightPos[0], simonGame_rightPos[1],
                            right_arrow_hollow, &right_arrow[2], 0, 0);
  Sprites::drawExternalMask(simonGame_upPos[0], simonGame_upPos[1],
                            up_arrow_hollow, &up_arrow[2], 0, 0);
  Sprites::drawExternalMask(simonGame_downPos[0], simonGame_downPos[1],
                            down_arrow_hollow, &down_arrow[2], 0, 0);
  Sprites::drawExternalMask(simonGame_APos[0], simonGame_APos[1], a_button,
                            &button_filled[2], 0, 0);
  Sprites::drawExternalMask(simonGame_BPos[0], simonGame_BPos[1], b_button,
                            &button_filled[2], 0, 0);
  seconds_elapsed = millis() / 1000 - simonGame_timer;

  // player's turn
  uint16_t pressed = simonGame_patternCount + 1;
  if (seconds_elapsed >= simonGame_patternCount) {
    if (arduboy.justPressed(LEFT_BUTTON)) {
      pressed = 0;
    } else if (arduboy.justPressed(RIGHT_BUTTON)) {
      pressed = 1;
    } else if (arduboy.justPressed(UP_BUTTON)) {
      pressed = 2;
    } else if (arduboy.justPressed(DOWN_BUTTON)) {
      pressed = 3;
    } else if (arduboy.justPressed(A_BUTTON)) {
      pressed = 4;
    } else if (arduboy.justPressed(B_BUTTON)) {
      pressed = 5;
    }

    simonGameLightUp(pressed);

    if (pressed == simonGame_pattern[simonGame_playerIdx]) {
      addScore(10);
      simonGame_playerIdx++;
      turnOnLED(COLOR_GREEN);
    } else if (pressed != simonGame_patternCount + 1) {
      turnOnLED(COLOR_RED);
      simonGame_failedState = true;
      simonGame_failedTime = millis() / 1000;
      return;
    }

    // time over or player got em all
    if (seconds_elapsed >= simonGame_patternCount + simonGame_playTimeSeconds ||
        simonGame_playerIdx >= simonGame_patternCount) {
      simonGame_newGame = true;
      gameState = STATE_INTERMISSION;
    }
    return;
  }

  // first play the pattern
  pattern_idx = simonGame_pattern[seconds_elapsed];
  simonGameLightUp(pattern_idx);
}
