#include "minigame_lockpick.h"

// Fast integer sine lookup table for 0 to 90 degrees.
// Values represent sin(angle) * 100.
const int8_t PROGMEM sin_table[91] = {
    0,  2,  3,  5,  7,   9,   10,  12,  14,  16,  17, 19, 21, 22, 24, 26,
    28, 29, 31, 33, 34,  36,  37,  39,  41,  42,  44, 45, 47, 48, 50, 52,
    53, 54, 56, 57, 59,  60,  62,  63,  64,  66,  67, 68, 69, 71, 72, 73,
    74, 75, 77, 78, 79,  80,  81,  82,  83,  84,  85, 86, 87, 87, 88, 89,
    90, 91, 91, 92, 93,  93,  94,  95,  95,  96,  96, 97, 97, 97, 98, 98,
    98, 99, 99, 99, 100, 100, 100, 100, 100, 100, 100};

// Returns an integer representing sin(degrees) * 100.
// Handles negative and positive values appropriately avoiding standard sin()
// overhead.
int8_t getSin8(int16_t degrees) {
  bool isNegative = false;

  if (degrees < 0) {
    degrees = -degrees;
    isNegative = true;
  }

  // Normalize to 0-359
  degrees %= 360;

  int8_t val;
  if (degrees <= 90) {
    val = pgm_read_byte(&sin_table[degrees]);
  } else if (degrees <= 180) {
    val = pgm_read_byte(&sin_table[180 - degrees]);
  } else if (degrees <= 270) {
    val = -pgm_read_byte(&sin_table[degrees - 180]);
  } else {
    val = -pgm_read_byte(&sin_table[360 - degrees]);
  }

  return isNegative ? -val : val;
}

// Returns an integer representing cos(degrees) * 100.
int8_t getCos8(int16_t degrees) { return getSin8(degrees + 90); }

bool lockpick_newGame = true;
unsigned long lockpick_timer = 0;
uint8_t lockpick_lastScoreSecond =
    0; // Tracks the last whole second we deducted points

// Angles are now represented in integer degrees
int16_t lockpick_pinAngle = 0; // 0 = straight up, ranges roughly -90 to 90
int16_t lockpick_targetAngle = 0;
int16_t lockpick_sweetSpotWidth = 15; // degrees of leniency
int16_t lockpick_cylinderAngle = 0;   // 0 = rest, 90 = fully turned
int16_t lockpick_maxTurn = 0;         // max turn aloud at current pin angle

void doLockpickGame() {
  if (lockpick_newGame) {
    lockpick_timer = millis();
    lockpick_lastScoreSecond = 0;
    lockpick_pinAngle = 0;
    lockpick_cylinderAngle = 0;

    // Target is between -70 and 70 degrees
    lockpick_targetAngle = random(-70, 71);

    // Grant initial 100 points
    addScore(100);

    lockpick_newGame = false;
  }

  // --- INPUT HANDLING ---

  // Left/Right adjust the pin IF the cylinder is at rest
  if (lockpick_cylinderAngle == 0) {
    if (arduboy.pressed(LEFT_BUTTON)) {
      lockpick_pinAngle -= 3;
    }
    if (arduboy.pressed(RIGHT_BUTTON)) {
      lockpick_pinAngle += 3;
    }
    // Clamp pin angle to top hemisphere (-90 to 90)
    if (lockpick_pinAngle < -90)
      lockpick_pinAngle = -90;
    if (lockpick_pinAngle > 90)
      lockpick_pinAngle = 90;
  }

  // A applies tension
  if (arduboy.pressed(A_BUTTON)) {
    // Determine how far the cylinder can turn based on pin proximity to target
    int16_t error = abs(lockpick_pinAngle - lockpick_targetAngle);
    if (error <= lockpick_sweetSpotWidth) {
      lockpick_maxTurn = 90; // Full 90 degrees
    } else {
      // Map error to a max turn amount. The closer, the further it turns.
      // E.g., if error is 30, and max theoretical error is ~180. We map it out.
      lockpick_maxTurn = 90 - (error / 2); // Roughly linear degradation
      if (lockpick_maxTurn < 5)
        lockpick_maxTurn = 5; // always turns a tiny bit
    }

    // Turn cylinder
    lockpick_cylinderAngle += 4;
    if (lockpick_cylinderAngle > lockpick_maxTurn) {
      lockpick_cylinderAngle = lockpick_maxTurn;
      // Visual shake handled in drawing logic
    }
  } else {
    // Release tension, cylinder springs back
    lockpick_cylinderAngle -= 10;
    if (lockpick_cylinderAngle < 0) {
      lockpick_cylinderAngle = 0;
    }
  }

  // --- TIMER & SCORING ---

  unsigned long elapsed = millis() - lockpick_timer;
  uint8_t currentSeconds = (uint8_t)(elapsed / 1000);

  if (currentSeconds > lockpick_lastScoreSecond && currentSeconds < 10) {
    addScore(-10);
    turnOnLED(COLOR_RED, 250); // Flash red LED & play sound every second
    lockpick_lastScoreSecond = currentSeconds;
  }

  // --- WIN/LOSS CONDITIONS ---

  if (lockpick_cylinderAngle >= 89) {
    // Win!
    turnOnLED(COLOR_GREEN, 1000);
    lockpick_newGame = true;
    gameState = STATE_INTERMISSION;
    return;
  }

  if (elapsed >= 10000) {
    // Out of time
    turnOnLED(COLOR_RED, 1000);
    lockpick_newGame = true;
    gameState = STATE_INTERMISSION;
    return;
  }

  // --- DRAWING ---

  // Center of lock mechanism
  int8_t cx = MAX_X_POS / 2;
  int8_t cy = MAX_Y_POS / 2 + 10; // offset down a bit
  int8_t radius = 20;

  // Draw Cylinder
  arduboy.drawCircle(cx, cy, radius, WHITE);
  arduboy.drawCircle(cx, cy, radius - 2, WHITE);

  // Calculate Keyhole rotation based on cylinder (cylinderAngle is 0 to 90)
  // Keyhole at rest is vertical.
  int16_t keyholeAngle = lockpick_cylinderAngle;
  int8_t kx1 = cx + (getSin8(keyholeAngle) * -10) / 100;
  int8_t ky1 = cy - (getCos8(keyholeAngle) * -10) / 100;
  int8_t kx2 = cx + (getSin8(keyholeAngle) * 10) / 100;
  int8_t ky2 = cy - (getCos8(keyholeAngle) * 10) / 100;
  arduboy.drawLine(kx1, ky1, kx2, ky2, WHITE);

  // Calculate Bobby Pin rotation (moves with cylinder tension!)
  int16_t currentPinAngle = lockpick_pinAngle + lockpick_cylinderAngle;

  // Add shake effect if tense and bound
  if (arduboy.pressed(A_BUTTON) &&
      abs(lockpick_cylinderAngle - lockpick_maxTurn) < 2 &&
      lockpick_maxTurn < 80) {
    // Alternate tiny shake based on frames
    if (arduboy.frameCount % 2 == 0) {
      currentPinAngle += 2;
    } else {
      currentPinAngle -= 2;
    }
  }

  int8_t px = cx + (getSin8(currentPinAngle) * 30) / 100;
  int8_t py = cy - (getCos8(currentPinAngle) * 30) / 100;

  arduboy.drawLine(cx, cy - 4, px, py,
                   WHITE); // offset base slightly above keyhole
}
