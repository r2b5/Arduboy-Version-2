#include "minigame_spotlight.h"

extern const unsigned char PROGMEM smiley_face[];

int16_t spotlightGame_slX = 64;
int16_t spotlightGame_slY = 32;
int16_t spotlightGame_fX = 0;
int16_t spotlightGame_fY = 0;
bool spotlightGame_newGame = true;
unsigned long spotlightGame_timer = 0;
unsigned long spotlightGame_overlapTime = 0;
bool spotlightGame_wasOverlapping = false;

void doSpotlightGame() {
  if (spotlightGame_newGame) {
    // Spawn smiley face
    spotlightGame_fX = random(0, 112); // Smiley top-left X
    spotlightGame_fY = random(0, 48);  // Smiley top-left Y

    // Spawn spotlight centered specifically over the smiley face
    // The smiley is 16x16, so its center is fX + 8, fY + 8
    spotlightGame_slX = spotlightGame_fX + 8;
    spotlightGame_slY = spotlightGame_fY + 8;

    spotlightGame_timer = millis();
    spotlightGame_overlapTime = 0;
    spotlightGame_wasOverlapping = false;
    spotlightGame_newGame = false;
  }

  // Input Movement
  int16_t speed = 2;
  if (arduboy.pressed(LEFT_BUTTON)) {
    spotlightGame_slX -= speed;
  }
  if (arduboy.pressed(RIGHT_BUTTON)) {
    spotlightGame_slX += speed;
  }
  if (arduboy.pressed(UP_BUTTON)) {
    spotlightGame_slY -= speed;
  }
  if (arduboy.pressed(DOWN_BUTTON)) {
    spotlightGame_slY += speed;
  }

  // Keep spotlight on screen
  if (spotlightGame_slX < 16)
    spotlightGame_slX = 16;
  if (spotlightGame_slX > 112)
    spotlightGame_slX = 112;
  if (spotlightGame_slY < 16)
    spotlightGame_slY = 16;
  if (spotlightGame_slY > 48)
    spotlightGame_slY = 48;

  // Draw the white spotlight
  arduboy.fillCircle(spotlightGame_slX, spotlightGame_slY, 16, WHITE);

  // Draw the smiley face in BLACK so it's only visible inside the white
  // spotlight
  arduboy.drawBitmap(spotlightGame_fX, spotlightGame_fY, smiley_face + 2, 16,
                     16, BLACK);

  // Check overlap: "overlaps at all" -> distance from rect to circle center <=
  // radius
  int16_t testX = spotlightGame_slX;
  int16_t testY = spotlightGame_slY;

  if (spotlightGame_slX < spotlightGame_fX)
    testX = spotlightGame_fX;
  else if (spotlightGame_slX > spotlightGame_fX + 16)
    testX = spotlightGame_fX + 16;

  if (spotlightGame_slY < spotlightGame_fY)
    testY = spotlightGame_fY;
  else if (spotlightGame_slY > spotlightGame_fY + 16)
    testY = spotlightGame_fY + 16;

  int16_t distX = spotlightGame_slX - testX;
  int16_t distY = spotlightGame_slY - testY;
  int32_t distanceSquared = (int32_t)(distX * distX) + (int32_t)(distY * distY);

  bool currentOverlap = (distanceSquared <= (16 * 16));

  if (currentOverlap) {
    if (!spotlightGame_wasOverlapping) {
      spotlightGame_overlapTime = millis();
      spotlightGame_wasOverlapping = true;
    } else {
      // Every 500ms, add 10 points
      if (millis() - spotlightGame_overlapTime >= 500) {
        addScore(10);
        sound.tone(1046, 100);
        spotlightGame_overlapTime += 500;
      }
    }
    arduboy.setRGBled(0, 255, 0); // Green

    static int16_t evadeVX = 0;
    static int16_t evadeVY = 0;
    static int evadeTimer = 0;

    // Averages 1.33, faster than base 1 but slower than 1.5
    int evasionSpeed = 1;
    if (random(5) == 0) {
      evasionSpeed = 2;
    }

    if (evadeTimer > 0) {
      evadeTimer--;
    } else {
      // Basic evasion: try to move away from the center of the spotlight
      evadeVX = 0;
      evadeVY = 0;
      if (spotlightGame_fX < spotlightGame_slX)
        evadeVX = -evasionSpeed;
      if (spotlightGame_fX > spotlightGame_slX)
        evadeVX = evasionSpeed;
      if (spotlightGame_fY < spotlightGame_slY)
        evadeVY = -evasionSpeed;
      if (spotlightGame_fY > spotlightGame_slY)
        evadeVY = evasionSpeed;

      // Add a bit of random wiggle
      if (random(2) == 0)
        evadeVX += random(-1, 2);
      if (random(2) == 0)
        evadeVY += random(-1, 2);
    }

    int nextX = spotlightGame_fX + evadeVX;
    int nextY = spotlightGame_fY + evadeVY;

    // Check if moving this way hits a border
    if (nextX <= 0 || nextX >= 112 || nextY <= 0 || nextY >= 48) {
      // Choose a random vector to continue evasion, avoiding the border
      evadeVX = random(-2, 3); // -2 to 2
      evadeVY = random(-2, 3);

      if (nextX <= 0)
        evadeVX = random(1, 4); // Move right
      if (nextX >= 112)
        evadeVX = random(-3, 0); // Move left
      if (nextY <= 0)
        evadeVY = random(1, 4); // Move down
      if (nextY >= 48)
        evadeVY = random(-3, 0); // Move up

      // Hold this evasion vector for a bit so it doesn't get stuck again
      // immediately
      evadeTimer = random(10, 30);
    }

    spotlightGame_fX += evadeVX;
    spotlightGame_fY += evadeVY;

    // Keep inside screen bounds
    if (spotlightGame_fX < 0)
      spotlightGame_fX = 0;
    if (spotlightGame_fX > 112)
      spotlightGame_fX = 112;
    if (spotlightGame_fY < 0)
      spotlightGame_fY = 0;
    if (spotlightGame_fY > 48)
      spotlightGame_fY = 48;

  } else {
    spotlightGame_wasOverlapping = false;
    arduboy.setRGBled(255, 0, 0); // Red
  }

  // 6-second timer check
  if (millis() - spotlightGame_timer >= 6100) {
    spotlightGame_newGame = true;
    arduboy.setRGBled(0, 0, 0); // Turn off LED
    gameState = STATE_INTERMISSION;
  }
}
