#include "minigame_rock.h"

extern const uint8_t PROGMEM a_button[];
extern const uint8_t PROGMEM b_button[];
extern const uint8_t PROGMEM button_filled[];
extern const uint8_t PROGMEM left_arrow[];
extern const uint8_t PROGMEM right_arrow[];

// Variables to keep track of state
bool rockGameInitialized = false;
unsigned long rockGameStartTime = 0;
unsigned long lastPressTime = 0;
int expectedButton = A_BUTTON; // Alternate starting from A, or could be either.
                               // Wait, user can start with either really, but
                               // let's say they have to start with A.
bool started = false;

// Variables for drawing the buttons
unsigned long aPressFlashTime = 0;
unsigned long bPressFlashTime = 0;

// Scoring tracking
unsigned long lastScoreSecond = 0;

void doRockGame() {
  if (!rockGameInitialized) {
    rockGameInitialized = true;
    rockGameStartTime = millis();
    lastPressTime = rockGameStartTime;
    expectedButton = 0; // 0 means either A or B can be pressed first
    started = false;
    aPressFlashTime = 0;
    bPressFlashTime = 0;
    lastScoreSecond = 0;

    // Clear the LEDs
    arduboy.setRGBled(0, 0, 0);
  }

  long timeRemaining = 6100 - (millis() - rockGameStartTime);

  // If game is over
  if (timeRemaining <= 0) {
    rockGameInitialized = false;
    gameState = STATE_INTERMISSION;
    arduboy.setRGBled(0, 0, 0); // turn off LED
    return;
  }

  // Handle Input
  bool buttonPressed = false;
  int pressedButton = 0;

  if (arduboy.justPressed(A_BUTTON)) {
    aPressFlashTime = millis();
    buttonPressed = true;
    pressedButton = A_BUTTON;
  } else if (arduboy.justPressed(B_BUTTON)) {
    bPressFlashTime = millis();
    buttonPressed = true;
    pressedButton = B_BUTTON;
  }

  unsigned long currentMillis = millis();

  // Logic for correct alternation
  bool isCorrect = false;

  if (buttonPressed) {
    started = true;
    if (expectedButton == 0) {
      // First press can be either
      lastPressTime = currentMillis;
      expectedButton = (pressedButton == A_BUTTON) ? B_BUTTON : A_BUTTON;
      isCorrect = true;
    } else if (pressedButton == expectedButton) {
      // Correct alternation
      lastPressTime = currentMillis;
      expectedButton = (pressedButton == A_BUTTON) ? B_BUTTON : A_BUTTON;
      isCorrect = true;
    } else {
      // Wrong button pressed
      isCorrect = false;
    }
  }

  // Determine LED status
  bool ShouldBeGreen = false;

  if (started) {
    // If they pressed button recently and haven't messed up alternation,
    // it's green. But we have to handle "what if they just pressed the wrong
    // one?" If they pressed the wrong one just now, they fail until they get it
    // right. Wait, if they compress the wrong one, we just don't accept it, or
    // do we reset? "as long as the button presses are alternating correctly and
    // a button has been pressed within the last half second, the LED should be
    // lit green"
    if (currentMillis - lastPressTime < 500) {
      ShouldBeGreen = true;
    } else {
      ShouldBeGreen = false;
    }

    // BUT what if they hit A, wait 200ms, hit A again? The second A violates
    // "alternating correctly". Does the LED go red immediately? Yes. Let's make
    // `started = false` or something? Let's refine: You must alternate. If you
    // fail to alternate (hit A then A again), you break the chain.
    if (buttonPressed && !isCorrect) {
      lastPressTime =
          0; // Force it to immediately look like it timed out to turn red
      ShouldBeGreen = false;
    }
  }

  // Score tracking per second
  unsigned long currentSecond = (millis() - rockGameStartTime) / 1000;
  bool secondRolledOver =
      (currentSecond > lastScoreSecond && currentSecond <= 6);
  if (currentSecond > lastScoreSecond) {
    lastScoreSecond = currentSecond;
  }

  // Set LED and Score
  if (ShouldBeGreen) {
    arduboy.setRGBled(0, 255, 0);
    if (secondRolledOver) {
      addScore(10);
      sound.tone(NOTE_C5, 100);
    }
  } else {
    arduboy.setRGBled(255, 0, 0);
  }

  // Draw Buttons
  // Screen is 128x64. Button sprites are 16x16.
  // We want them centered vertically (64-16)/2 = 24
  // And spaced evenly horizontally. Let's place A at x=32 and B at x=80.
  int xPosLeft = 32;
  int xPosRight = 80;
  int yPos = 24;

  // Draw A Button
  if (currentMillis - aPressFlashTime < 100) {
    Sprites::drawExternalMask(xPosLeft, yPos, button_filled, &button_filled[2],
                              0, 0);
  } else {
    Sprites::drawExternalMask(xPosLeft, yPos, a_button, &button_filled[2], 0,
                              0);
  }

  // Draw B Button
  if (currentMillis - bPressFlashTime < 100) {
    Sprites::drawExternalMask(xPosRight, yPos, button_filled, &button_filled[2],
                              0, 0);
  } else {
    Sprites::drawExternalMask(xPosRight, yPos, b_button, &button_filled[2], 0,
                              0);
  }

  // Draw arrow between buttons. Arrow sprites are 16x16.
  // Center of screen is x=64, y=32. Arrow top-left should be (64-8)=56,
  // (32-8)=24
  if (expectedButton == A_BUTTON || expectedButton == 0) {
    Sprites::drawExternalMask(56, 24, left_arrow, &left_arrow[2], 0, 0);
  } else if (expectedButton == B_BUTTON) {
    Sprites::drawExternalMask(56, 24, right_arrow, &right_arrow[2], 0, 0);
  }
}
