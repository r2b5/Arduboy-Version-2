#include "minigame_spacedodge.h"

bool spaceDodge_newGame = true;
unsigned long spaceDodge_timer = 0;
int16_t spaceDodge_shipX = 64 - 8;
int16_t spaceDodge_shipY = 32 - 8;
int spaceDodge_scoreSeconds = 0;

struct Asteroid {
  float x;
  float y;
  float vx;
  float vy;
  uint8_t sizeType;
  bool hitPlayer;
};

Asteroid asteroids[6];

void spawnAsteroid(int i) {
  int edge = random(4); // 0=top, 1=right, 2=bottom, 3=left
  asteroids[i].sizeType = random(4);
  int sInfo[] = {4, 8, 12, 16};
  int s = sInfo[asteroids[i].sizeType];

  if (edge == 0) { // Top
    asteroids[i].x = random(0, 128);
    asteroids[i].y = -s;
    asteroids[i].vx = random(-15, 20) / 10.0;
    asteroids[i].vy = random(10, 25) / 10.0;
  } else if (edge == 1) { // Right
    asteroids[i].x = 128 + s;
    asteroids[i].y = random(0, 64);
    asteroids[i].vx = -random(10, 25) / 10.0;
    asteroids[i].vy = random(-15, 20) / 10.0;
  } else if (edge == 2) { // Bottom
    asteroids[i].x = random(0, 128);
    asteroids[i].y = 64 + s;
    asteroids[i].vx = random(-15, 20) / 10.0;
    asteroids[i].vy = -random(10, 25) / 10.0;
  } else { // Left
    asteroids[i].x = -s;
    asteroids[i].y = random(0, 64);
    asteroids[i].vx = random(10, 25) / 10.0;
    asteroids[i].vy = random(-15, 20) / 10.0;
  }

  if (asteroids[i].vx == 0 && asteroids[i].vy == 0) {
    asteroids[i].vx = 1.0;
  }
  asteroids[i].hitPlayer = false;
}

void doSpaceDodgeGame() {
  if (spaceDodge_newGame) {
    spaceDodge_timer = millis();
    spaceDodge_scoreSeconds = 0;
    spaceDodge_shipX = 64 - 8;
    spaceDodge_shipY = 32 - 8;

    for (int i = 0; i < 6; i++) {
      spawnAsteroid(i);
    }

    // Grant initial 60 points
    addScore(60);

    spaceDodge_newGame = false;
  }

  // Ship Control
  int16_t speed = 2;
  if (arduboy.pressed(LEFT_BUTTON))
    spaceDodge_shipX -= speed;
  if (arduboy.pressed(RIGHT_BUTTON))
    spaceDodge_shipX += speed;
  if (arduboy.pressed(UP_BUTTON))
    spaceDodge_shipY -= speed;
  if (arduboy.pressed(DOWN_BUTTON))
    spaceDodge_shipY += speed;

  // Bounds checking for ship
  if (spaceDodge_shipX < 0)
    spaceDodge_shipX = 0;
  if (spaceDodge_shipX > 112)
    spaceDodge_shipX = 112;
  if (spaceDodge_shipY < 0)
    spaceDodge_shipY = 0;
  if (spaceDodge_shipY > 48)
    spaceDodge_shipY = 48;

  bool hit = false;

  // Asteroid Updates
  for (int i = 0; i < 6; i++) {
    int astSize = 0;
    const uint8_t *astSprite = 0;
    if (asteroids[i].sizeType == 0) {
      astSize = 4;
      astSprite = asteroid_4;
    } else if (asteroids[i].sizeType == 1) {
      astSize = 8;
      astSprite = asteroid_8;
    } else if (asteroids[i].sizeType == 2) {
      astSize = 12;
      astSprite = asteroid_12;
    } else if (asteroids[i].sizeType == 3) {
      astSize = 16;
      astSprite = asteroid_16;
    }

    asteroids[i].x += asteroids[i].vx;
    asteroids[i].y += asteroids[i].vy;

    // Check bounds to respawn
    if (asteroids[i].x < -32 || asteroids[i].x > 160 || asteroids[i].y < -32 ||
        asteroids[i].y > 96) {
      spawnAsteroid(i);
    }

    // Draw
    arduboy.drawBitmap((int16_t)asteroids[i].x, (int16_t)asteroids[i].y,
                       astSprite + 2, astSize, astSize, WHITE);

    // Check hit (AABB with a forgiving margin)
    int hitboxMargin = 2;
    int astMargin =
        astSize > 4 ? 2 : 0; // Don't give margins for the tiny 4x4 asteroid
    if (!asteroids[i].hitPlayer &&
        spaceDodge_shipX + hitboxMargin <
            (int16_t)asteroids[i].x + astSize - astMargin &&
        spaceDodge_shipX + 16 - hitboxMargin >
            (int16_t)asteroids[i].x + astMargin &&
        spaceDodge_shipY + hitboxMargin <
            (int16_t)asteroids[i].y + astSize - astMargin &&
        spaceDodge_shipY + 16 - hitboxMargin >
            (int16_t)asteroids[i].y + astMargin) {

      asteroids[i].hitPlayer = true;
      addScore(-10);
      turnOnLED(COLOR_RED, 250); // Red LED and fail sound
    }
  }

  // Draw ship
  arduboy.drawBitmap(spaceDodge_shipX, spaceDodge_shipY, spaceship + 2, 16, 16,
                     WHITE);

  // Check endings
  int currentSecond = (millis() - spaceDodge_timer) / 1000;
  if (currentSecond >= 6) {
    turnOffLED();
    spaceDodge_newGame = true;
    gameState = STATE_INTERMISSION;
  }
}
