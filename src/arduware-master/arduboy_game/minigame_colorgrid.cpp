#include "minigame_colorgrid.h"

// Local constants for colors
const uint8_t CG_RED = 0;
const uint8_t CG_GREEN = 1;
const uint8_t CG_BLUE = 2;
const uint8_t CG_YELLOW = 3;
const uint8_t CG_CYAN = 4;
const uint8_t CG_MAGENTA = 5;
const uint8_t CG_EMPTY = 6;

// Text names for the colors
const char str_red[] PROGMEM = "RED";
const char str_green[] PROGMEM = "GREEN";
const char str_blue[] PROGMEM = "BLUE";
const char str_yellow[] PROGMEM = "YELLOW";
const char str_cyan[] PROGMEM = "CYAN";
const char str_magenta[] PROGMEM = "MAGENTA";

const char *const colorNames[] PROGMEM = {str_red,    str_green, str_blue,
                                          str_yellow, str_cyan,  str_magenta};

// Game state variables
static bool isInitialized = false;
static uint8_t grid[16]; // 4x4 grid flattened
static uint8_t targetColor;
static int8_t cursorX = 0;
static int8_t cursorY = 0;
static unsigned long gameStartTime = 0;
static const unsigned long GAME_DURATION = 8000; // 8 seconds

static void setLEDForColor(uint8_t color) {
  switch (color) {
  case CG_RED:
    arduboy.setRGBled(255, 0, 0);
    break;
  case CG_GREEN:
    arduboy.setRGBled(0, 255, 0);
    break;
  case CG_BLUE:
    arduboy.setRGBled(0, 0, 255);
    break;
  case CG_YELLOW:
    arduboy.setRGBled(255, 255, 0);
    break;
  case CG_CYAN:
    arduboy.setRGBled(0, 255, 255);
    break;
  case CG_MAGENTA:
    arduboy.setRGBled(255, 0, 255);
    break;
  default:
    arduboy.setRGBled(0, 0, 0);
    break;
  }
}

static void initColorGrid() {
  targetColor = random(6);
  cursorX = 0;
  cursorY = 0;
  gameStartTime = millis();

  // Fill exactly 6 squares with target color
  for (uint8_t i = 0; i < 6; i++) {
    grid[i] = targetColor;
  }

  // Fill the remaining 10 with random non-target colors
  for (uint8_t i = 6; i < 16; i++) {
    uint8_t randomColor = random(5);
    if (randomColor >= targetColor) {
      randomColor++; // Skip the target color
    }
    grid[i] = randomColor;
  }

  // Shuffle the grid
  for (uint8_t i = 15; i > 0; i--) {
    uint8_t j = random(i + 1);
    uint8_t temp = grid[i];
    grid[i] = grid[j];
    grid[j] = temp;
  }

  isInitialized = true;
}

void doColorGridGame() {
  if (!isInitialized) {
    initColorGrid();
  }

  // Check timer
  if (millis() - gameStartTime >= GAME_DURATION) {
    isInitialized = false;
    arduboy.setRGBled(0, 0, 0); // Turn off LED
    gameState = STATE_INTERMISSION;
    return;
  }

  // Input handling
  if (arduboy.justPressed(UP_BUTTON)) {
    cursorY--;
    if (cursorY < 0)
      cursorY = 3;
  }
  if (arduboy.justPressed(DOWN_BUTTON)) {
    cursorY++;
    if (cursorY > 3)
      cursorY = 0;
  }
  if (arduboy.justPressed(LEFT_BUTTON)) {
    cursorX--;
    if (cursorX < 0)
      cursorX = 3;
  }
  if (arduboy.justPressed(RIGHT_BUTTON)) {
    cursorX++;
    if (cursorX > 3)
      cursorX = 0;
  }

  uint8_t cursorIndex = cursorY * 4 + cursorX;

  if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) {
    if (grid[cursorIndex] != CG_EMPTY) {
      if (grid[cursorIndex] == targetColor) {
        addScore(10);
        sound.tone(1046, 100); // Success sound
        grid[cursorIndex] = CG_EMPTY;
      } else {
        if (score >= 10) {
          addScore(-10);
        } else {
          addScore(-score);
        }
        sound.tone(130, 250); // Fail sound
        grid[cursorIndex] = CG_EMPTY;
      }
    }
  }

  // Update LED continuously based on current selection color
  // Only override if the global LED isn't managing a success/fail beep
  // Wait, prompt says: "dont light the LED for success or fail indications as
  // it will confuse the player." So we just set it strictly to the hovered
  // square's color.
  setLEDForColor(grid[cursorIndex]);

  // Rendering
  // Target color text
  arduboy.setCursor(30, 10);
  arduboy.print(F("FIND: "));
  char colorBuffer[10];
  strcpy_P(colorBuffer, (char *)pgm_read_word(&(colorNames[targetColor])));
  arduboy.print(colorBuffer);

  // Render the grid
  // Math for centering a 4x4 grid of squares. Let's make each square 8x8 with
  // 2px gap. Totals: (8+2)*4 - 2 = 38px width/height. Center X: (128 - 38)/2
  // = 45. Center Y: (64 - 38)/2 = 13 + some offset for text: let's do 20.
  const int startX = 45;
  const int startY = 22;
  const int squareSize = 8;
  const int spacing = 10; // 8 size + 2 gap

  for (uint8_t y = 0; y < 4; y++) {
    for (uint8_t x = 0; x < 4; x++) {
      uint8_t index = y * 4 + x;
      int drawX = startX + x * spacing;
      int drawY = startY + y * spacing;

      // Draw the squares
      if (grid[index] != CG_EMPTY) {
        // We draw filled rectangles. To distinguish colors on monochrome, we
        // can use different patterns? Wait, the player relies on the RGB LED to
        // know the color! Oh, this is a clever design! We just draw all solid
        // squares, and only the highlighted one will light up the LED. Wait,
        // the player can only see the color by moving the cursor to it! "when a
        // square is highlighted, light up the LED the color that the square
        // represents" Yes, this is exactly what the prompt asked for.

        arduboy.fillRect(drawX, drawY, squareSize, squareSize, WHITE);
      } else {
        // Collected squares could be an X or just a dot
        arduboy.drawPixel(drawX + squareSize / 2, drawY + squareSize / 2,
                          WHITE);
      }

      // Highlight cursor
      if (x == cursorX && y == cursorY) {
        // Draw an outer boundary or an inverted box
        arduboy.drawRect(drawX - 1, drawY - 1, squareSize + 2, squareSize + 2,
                         WHITE);
        if (grid[index] != CG_EMPTY) {
          // Inner dot or something?
        }
      }
    }
  }

  // Draw time remaining bar
  unsigned long timeRemaining = GAME_DURATION - (millis() - gameStartTime);
  int barWidth = (128L * timeRemaining) / GAME_DURATION;
  arduboy.drawFastHLine(0, 62, barWidth, WHITE);
}
