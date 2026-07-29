#include "minigame_hot.h"

float makeItHot_progress = 0;
bool makeItHot_newGame = true;
unsigned long makeItHot_timer = 0;
uint8_t makeItHot_sectionsPassed = 0;

void doMakeItHotGame() {

  if (makeItHot_newGame) {
    makeItHot_progress = 0;
    makeItHot_timer = millis();
    makeItHot_sectionsPassed = 0;
    makeItHot_newGame = false;
  }

  // Calculate remaining time
  int time_elapsed = (millis() - makeItHot_timer) / 1000;
  int time_remaining = 6 - time_elapsed;

  // Title text
  arduboy.setCursor(14, 10);
  arduboy.print(F("Smash the A button!"));

  // Timer text
  arduboy.setCursor(16, 30);
  arduboy.print(time_remaining);
  arduboy.print(F("s"));

  // Thermometer outline
  int therm_x = 60;
  int therm_y = 20;
  int therm_w = 8;
  int therm_h = 32;

  // Bulb at the bottom
  int bulb_r = 6;
  int bulb_x = therm_x + (therm_w / 2);
  int bulb_y = therm_y + therm_h + 2;

  // Draw thermometer outline
  arduboy.drawRect(therm_x, therm_y, therm_w, therm_h, WHITE);
  arduboy.drawCircle(bulb_x, bulb_y, bulb_r, WHITE);
  // Clear the line between the bulb and the stem
  arduboy.drawLine(therm_x + 1, therm_y + therm_h, therm_x + therm_w - 2,
                   therm_y + therm_h, BLACK);

  // Handle Input
  if (arduboy.justPressed(A_BUTTON)) {
    makeItHot_progress +=
        4.5f; // Each press fills it up by this much percentage
  }

  // Constantly drain the thermometer
  makeItHot_progress -= 0.5f; // Drain rate

  if (makeItHot_progress < 0) {
    makeItHot_progress = 0;
  } else if (makeItHot_progress > 100) {
    makeItHot_progress = 100;
  }

  // Draw the filled portion of the thermometer
  // For the bulb, we can just fill it for simplicity, since progress starts
  // there
  arduboy.fillCircle(bulb_x, bulb_y, bulb_r - 1, WHITE);

  // Calculate the height of the filled stem based on progress
  int fill_h = (makeItHot_progress / 100.0f) * (therm_h - 1);
  if (fill_h > 0) {
    int fill_y = therm_y + therm_h - fill_h;
    arduboy.fillRect(therm_x + 1, fill_y, therm_w - 2, fill_h, WHITE);
  }

  // Draw section lines with dynamic colors
  // The line is covered if the top of the fluid (fill_y) is less than or equal
  // to the line's Y pos
  int fill_y = therm_y + therm_h - fill_h;
  arduboy.drawFastHLine(therm_x + 1, therm_y + 24, therm_w - 2,
                        (fill_h > 0 && fill_y <= therm_y + 24) ? BLACK : WHITE);
  arduboy.drawFastHLine(therm_x + 1, therm_y + 16, therm_w - 2,
                        (fill_h > 0 && fill_y <= therm_y + 16) ? BLACK : WHITE);
  arduboy.drawFastHLine(therm_x + 1, therm_y + 8, therm_w - 2,
                        (fill_h > 0 && fill_y <= therm_y + 8) ? BLACK : WHITE);

  // Check section thresholds
  int current_section = makeItHot_progress / 25;
  if (current_section > 4)
    current_section = 4; // cap at 4

  if (current_section > makeItHot_sectionsPassed) {
    // Award 10 points for every new section reached live
    int sections_new = current_section - makeItHot_sectionsPassed;
    addScore(sections_new * 10);

    makeItHot_sectionsPassed = current_section;
    turnOnLED(COLOR_GREEN, 1000);
  }

  // End condition: run out of time
  if (time_remaining <= 0) {
    makeItHot_newGame = true;
    gameState = STATE_INTERMISSION;
    return;
  }
}
