#include "minigame_duckhunt.h"

// Variables for Duck Hunt
bool duckHunt_newGame = true;
unsigned long duckHunt_timer = 0;

int16_t duck_crosshairX = 64;
int16_t duck_crosshairY = 32;

struct Duck {
  bool active;
  int16_t x;
  int16_t y;
  int16_t vx;
  int16_t vy;
  uint8_t evadeTimer;
};

Duck ducks[4];

void doDuckHuntGame() {
  if (duckHunt_newGame) {
    duckHunt_timer = millis();
    duck_crosshairX = 64 - 8;
    duck_crosshairY = 32 - 8;

    for (int i = 0; i < 4; i++) {
      ducks[i].active = true;
      ducks[i].x = random(0, 112);
      ducks[i].y = random(0, 48);
      ducks[i].vx = (random(4) == 0 ? 2 : 1) * (random(2) == 0 ? 1 : -1);
      ducks[i].vy = (random(4) == 0 ? 2 : 1) * (random(2) == 0 ? 1 : -1);
      ducks[i].evadeTimer = random(10, 30);
    }
    duckHunt_newGame = false;
  }

  // Crosshair Movement
  int16_t speed = 3;
  if (arduboy.pressed(LEFT_BUTTON)) {
    duck_crosshairX -= speed;
  }
  if (arduboy.pressed(RIGHT_BUTTON)) {
    duck_crosshairX += speed;
  }
  if (arduboy.pressed(UP_BUTTON)) {
    duck_crosshairY -= speed;
  }
  if (arduboy.pressed(DOWN_BUTTON)) {
    duck_crosshairY += speed;
  }

  // Keep crosshair on screen
  if (duck_crosshairX < 0)
    duck_crosshairX = 0;
  if (duck_crosshairX > 112)
    duck_crosshairX = 112;
  if (duck_crosshairY < 0)
    duck_crosshairY = 0;
  if (duck_crosshairY > 48)
    duck_crosshairY = 48;

  // Duck Movement and Evasion
  for (int i = 0; i < 4; i++) {
    if (!ducks[i].active)
      continue;

    if (ducks[i].evadeTimer > 0) {
      ducks[i].evadeTimer--;
    } else {
      // Evade crosshair based on distance
      int16_t distX = ducks[i].x - duck_crosshairX;
      int16_t distY = ducks[i].y - duck_crosshairY;
      int evasionSpeed = (random(4) == 0 ? 2 : 1);

      ducks[i].vx = (distX > 0) ? evasionSpeed : -evasionSpeed;
      ducks[i].vy = (distY > 0) ? evasionSpeed : -evasionSpeed;

      // Random wiggle
      if (random(2) == 0)
        ducks[i].vx += random(-1, 2);
      if (random(2) == 0)
        ducks[i].vy += random(-1, 2);

      ducks[i].evadeTimer = random(10, 30);
    }

    int nextX = ducks[i].x + ducks[i].vx;
    int nextY = ducks[i].y + ducks[i].vy;

    // Bounce off walls
    if (nextX <= 0 || nextX >= 112) {
      ducks[i].vx = -ducks[i].vx;
      if (ducks[i].vx == 0)
        ducks[i].vx = (nextX <= 0) ? 2 : -2;
      ducks[i].evadeTimer = random(10, 30);
    }
    if (nextY <= 0 || nextY >= 48) {
      ducks[i].vy = -ducks[i].vy;
      if (ducks[i].vy == 0)
        ducks[i].vy = (nextY <= 0) ? 2 : -2;
      ducks[i].evadeTimer = random(10, 30);
    }

    ducks[i].x += ducks[i].vx;
    ducks[i].y += ducks[i].vy;

    // Bound firmly
    if (ducks[i].x < 0)
      ducks[i].x = 0;
    if (ducks[i].x > 112)
      ducks[i].x = 112;
    if (ducks[i].y < 0)
      ducks[i].y = 0;
    if (ducks[i].y > 48)
      ducks[i].y = 48;
  }

  // Draw ducks
  for (int i = 0; i < 4; i++) {
    if (!ducks[i].active)
      continue;

    if (ducks[i].vx < 0) {
      arduboy.drawBitmap(ducks[i].x, ducks[i].y, duck_left + 2, 16, 16, WHITE);
    } else {
      arduboy.drawBitmap(ducks[i].x, ducks[i].y, duck_right + 2, 16, 16, WHITE);
    }
  }

  // Draw Crosshairs
  arduboy.drawBitmap(duck_crosshairX, duck_crosshairY, crosshairs + 2, 16, 16,
                     WHITE);

  // Shooting Mechanic
  if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) {
    int crossCenterX = duck_crosshairX + 8;
    int crossCenterY = duck_crosshairY + 8;
    bool hit = false;

    for (int i = 0; i < 4; i++) {
      if (ducks[i].active) {
        if (crossCenterX >= ducks[i].x && crossCenterX <= ducks[i].x + 16 &&
            crossCenterY >= ducks[i].y && crossCenterY <= ducks[i].y + 16) {

          ducks[i].active = false;
          hit = true;
          break; // only hit one duck per shot
        }
      }
    }

    if (hit) {
      addScore(10);
      turnOnLED(COLOR_GREEN,
                1000); // 1 second green LED with standard success sound
    } else {
      addScore(-5);
      turnOnLED(COLOR_RED,
                1000); // 1 second red LED with standard failure sound
    }
  }

  // 6-second timer check
  if (millis() - duckHunt_timer >= 6000) {
    duckHunt_newGame = true;
    turnOffLED();
    gameState = STATE_INTERMISSION;
  }
}
