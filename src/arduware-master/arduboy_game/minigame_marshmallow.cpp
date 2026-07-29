#include "minigame_marshmallow.h"

struct Marshmallow {
  float x;
  float y;
  float speed;
  bool active;
};

int16_t md_mugX = 56;
const int md_mugW = 16;
const int md_mugH = 12;

Marshmallow md_marshmallows[6];
int md_spawnedCount = 0;
int md_resolvedCount = 0;
unsigned long md_lastSpawnTime = 0;
unsigned long md_nextSpawnDelay = 0;

bool md_newGame = true;
unsigned long md_endTimer = 0;

void doMarshmallowDropGame() {
  if (md_newGame) {
    md_mugX = 56;
    md_spawnedCount = 0;
    md_resolvedCount = 0;
    md_lastSpawnTime = millis();
    md_nextSpawnDelay = random(500, 1501); // Initial spawn delay 0.5s - 1.5s
    md_endTimer = 0;
    for (int i = 0; i < 6; i++) {
      md_marshmallows[i].active = false;
    }
    md_newGame = false;
  }

  // Handle endgame state
  if (md_resolvedCount >= 6) {
    md_newGame = true;
    gameState = STATE_INTERMISSION;
    return;
  }

  // Mug Input (continuous)
  if (arduboy.pressed(LEFT_BUTTON)) {
    md_mugX -= 2;
  }
  if (arduboy.pressed(RIGHT_BUTTON)) {
    md_mugX += 2;
  }

  // Wrap-around Mug
  if (md_mugX < -md_mugW + 1) {
    md_mugX = 127;
  } else if (md_mugX > 127) {
    md_mugX = -md_mugW + 1;
  }

  // Draw Mug
  int mugY = 64 - md_mugH;
  // Left edge
  arduboy.drawLine(md_mugX, mugY + 1, md_mugX, 63, WHITE);
  // Right edge
  arduboy.drawLine(md_mugX + md_mugW - 1, mugY + 1, md_mugX + md_mugW - 1, 63,
                   WHITE);
  // Bottom edge
  arduboy.drawLine(md_mugX, 63, md_mugX + md_mugW - 1, 63, WHITE);
  // Lip of the mug (extends slightly past the edges)
  arduboy.drawLine(md_mugX - 2, mugY, md_mugX + md_mugW + 1, mugY, WHITE);
  // Handle on the left side
  arduboy.drawRect(md_mugX - 4, mugY + 3, 4, 6, WHITE);

  // Animated steam
  int yOffset = (arduboy.frameCount / 2) % 4;
  for (int i = 0; i < 3; i++) {
    int steamX = md_mugX + 4 + (i * 4);
    int steamPhase = (arduboy.frameCount / 6 + i) % 2;
    int sY = mugY - 2 - yOffset;

    if (steamPhase == 0) {
      arduboy.drawLine(steamX, sY, steamX + 1, sY - 3, WHITE);
      arduboy.drawLine(steamX + 1, sY - 4, steamX, sY - 7, WHITE);
    } else {
      arduboy.drawLine(steamX + 1, sY, steamX, sY - 3, WHITE);
      arduboy.drawLine(steamX, sY - 4, steamX + 1, sY - 7, WHITE);
    }
  }

  // Spawning logic
  if (md_spawnedCount < 6 &&
      (millis() - md_lastSpawnTime >= md_nextSpawnDelay)) {
    // Find an inactive marshmallow to spawn
    for (int i = 0; i < 6; i++) {
      if (!md_marshmallows[i].active) {
        md_marshmallows[i].active = true;
        md_marshmallows[i].x = random(0, 128 - 4); // 4 is marshmallow width
        md_marshmallows[i].y = -4; // Spawn slightly above screen
        md_marshmallows[i].speed =
            random(5, 16) / 10.0f; // 0.5 to 1.5 pixels per frame

        md_spawnedCount++;
        md_lastSpawnTime = millis();
        // Wait 0.5 to 1.5 seconds before spawning next
        if (md_spawnedCount < 6) {
          md_nextSpawnDelay = random(500, 1501);
        }
        break; // Only spawn one this frame
      }
    }
  }

  // Marshmallow Update & Draw
  for (int i = 0; i < 6; i++) {
    if (md_marshmallows[i].active) {
      md_marshmallows[i].y += md_marshmallows[i].speed;

      int mX = (int)md_marshmallows[i].x;
      int mY = (int)md_marshmallows[i].y;

      // Draw marshmallow (solid rect taller than wide, e.g. 4x6)
      arduboy.fillRect(mX, mY, 4, 6, WHITE);

      // Collision Check (AABB with the opening of the mug)
      // The "catching" area is roughly the top of the mug
      if (mY + 6 >= mugY && mY < 64 && mX + 4 > md_mugX &&
          mX < md_mugX + md_mugW) {
        // Caught!
        addScore(10);
        turnOnLED(COLOR_GREEN);
        md_marshmallows[i].active = false;
        md_resolvedCount++;
      }
      // Missed (reached bottom)
      else if (mY >= 64) {
        turnOnLED(COLOR_RED);
        md_marshmallows[i].active = false;
        md_resolvedCount++;
      }
    }
  }
}
