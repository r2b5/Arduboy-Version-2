#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <EEPROM.h>
#include <stdlib.h>

#include "arduware_title_compressed.h"
#include "globals.h"
#include "minigame_arrows.h"
#include "minigame_colorgrid.h"
#include "minigame_duckhunt.h"
#include "minigame_hot.h"
#include "minigame_hurdles.h"
#include "minigame_lockpick.h"
#include "minigame_marshmallow.h"
#include "minigame_redlightgreenlight.h"
#include "minigame_simon.h"
#include "minigame_spacedodge.h"
#include "minigame_spotlight.h"

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);

GameMode currentGameMode = MODE_SURVIVAL;
int score = 0;
int playerHealth = 100;
int minigamesSurvived = 0;
int minigamePointsEarned = 0;
uint8_t exitTimer = 0;

void addScore(int points) {
  if (currentGameMode == MODE_SCORE) {
    score += points;
    if (score < 0)
      score = 0;
  }
  minigamePointsEarned += points;
}

const unsigned char PROGMEM background[] = {
    // width, height,
    8, 8, 0x81, 0x00, 0x12, 0x40, 0x04, 0x11, 0x00, 0x04,
};

const uint8_t PROGMEM right_arrow[] = {
    16,   16,   0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
    0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x1f, 0x0f, 0x07, 0x03, 0x01, 0x00,
};

const uint8_t PROGMEM left_arrow[] = {
    16,   16,   0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xc0, 0xc0, 0xc0, 0xc0,
    0xc0, 0xc0, 0xc0, 0xc0, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00,
};

const uint8_t PROGMEM down_arrow[] = {
    16,   16,   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xfc, 0xfc, 0xfc,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x0c, 0x1c,
    0x3f, 0x7f, 0x7f, 0x3f, 0x1c, 0x0c, 0x04, 0x00, 0x00, 0x00,
};

const uint8_t PROGMEM up_arrow[] = {
    16,   16,   0x00, 0x00, 0x00, 0x20, 0x30, 0x38, 0xfc, 0xfe, 0xfe, 0xfc,
    0x38, 0x30, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x3f, 0x3f, 0x3f, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const uint8_t PROGMEM right_arrow_hollow[] = {
    16,   16,   0x00, 0x00, 0xc0, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x78, 0x10, 0x20, 0x40, 0x80, 0x00, 0x00, 0x00, 0x03, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x1e, 0x08, 0x04, 0x02, 0x01, 0x00,
};

const uint8_t PROGMEM left_arrow_hollow[] = {
    16,   16,   0x00, 0x80, 0x40, 0x20, 0x10, 0x78, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0xc0, 0x00, 0x00, 0x00, 0x01, 0x02, 0x04, 0x08, 0x1e,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x00, 0x00,
};

const uint8_t PROGMEM down_arrow_hollow[] = {
    16,   16,   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x04, 0x04, 0xfc,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x0c, 0x14,
    0x27, 0x40, 0x40, 0x27, 0x14, 0x0c, 0x04, 0x00, 0x00, 0x00,
};

const uint8_t PROGMEM up_arrow_hollow[] = {
    16,   16,   0x00, 0x00, 0x00, 0x20, 0x30, 0x28, 0xe4, 0x02, 0x02, 0xe4,
    0x28, 0x30, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x3f, 0x20, 0x20, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const uint8_t PROGMEM a_button[] = {
    16,   16,   0xe0, 0x30, 0x18, 0x0c, 0x06, 0xe3, 0x31, 0x11, 0x11, 0x31,
    0xe3, 0x06, 0x0c, 0x18, 0x30, 0xe0, 0x03, 0x06, 0x0c, 0x18, 0x30, 0x67,
    0x41, 0x41, 0x41, 0x41, 0x67, 0x30, 0x18, 0x0c, 0x06, 0x03,
};

const uint8_t PROGMEM b_button[] = {
    16,   16,   0xe0, 0x30, 0x18, 0x0c, 0x06, 0x03, 0xf1, 0x91, 0x91, 0x91,
    0x63, 0x06, 0x0c, 0x18, 0x30, 0xe0, 0x03, 0x06, 0x0c, 0x18, 0x30, 0x60,
    0x47, 0x44, 0x44, 0x44, 0x63, 0x30, 0x18, 0x0c, 0x06, 0x03,
};

const uint8_t PROGMEM button_filled[] = {
    16,   16,   0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f,
    0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03,
};

const uint8_t PROGMEM duck_right[] = {
    16,   16,   0x00, 0x00, 0x80, 0xc0, 0xe0, 0xe0, 0xe0, 0x7e, 0x7e, 0x3a,
    0x3e, 0x7c, 0xf8, 0xf8, 0x70, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x0f,
    0x0f, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const uint8_t PROGMEM duck_left[] = {
    16,   16,   0x00, 0x70, 0xf8, 0xf8, 0x7c, 0x3e, 0x3a, 0x7e, 0x7e, 0xe0,
    0xe0, 0xe0, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x03, 0x07, 0x0f, 0x0f, 0x07, 0x03, 0x01, 0x00, 0x00};

const uint8_t PROGMEM crosshairs[] = {
    16,   16,   0xc0, 0xb0, 0x88, 0x84, 0x82, 0x82, 0x01, 0x3f, 0x3f, 0x01,
    0x82, 0x82, 0x84, 0x88, 0xb0, 0xc0, 0x03, 0x0d, 0x11, 0x21, 0x41, 0x41,
    0x80, 0xfc, 0xfc, 0x80, 0x41, 0x41, 0x21, 0x11, 0x0d, 0x03};

const uint8_t PROGMEM spaceship[] = {
    16,   16,   0x00, 0x80, 0x80, 0xc0, 0xc0, 0xe0, 0xf0, 0xf8, 0xf8, 0xf0,
    0xe0, 0xc0, 0xc0, 0x80, 0x80, 0x00, 0x00, 0x24, 0x24, 0x24, 0x24, 0x24,
    0x3c, 0x3f, 0x3f, 0x3c, 0x24, 0x24, 0x24, 0x24, 0x24, 0x00};

const uint8_t PROGMEM asteroid_4[] = {4, 4, 0x06, 0x0f, 0x0f, 0x06};

const uint8_t PROGMEM asteroid_8[] = {8,    8,    0x3c, 0x7e, 0xff,
                                      0xdf, 0xef, 0xff, 0x7e, 0x3c};

const uint8_t PROGMEM asteroid_12[] = {12,   12,   0xf0, 0xf8, 0xfc, 0xfe, 0x7e,
                                       0xfe, 0xfe, 0xfc, 0xf8, 0xfc, 0xf8, 0xf0,
                                       0x03, 0x07, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
                                       0x0f, 0x07, 0x07, 0x03, 0x03};

const uint8_t PROGMEM asteroid_16[] = {
    16,   16,   0xe0, 0xf8, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff,
    0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0x03, 0x0f, 0x1f, 0x3f, 0x7f, 0x7f,
    0x7f, 0xff, 0x7f, 0x7f, 0x3f, 0x3f, 0x1f, 0x0f, 0x0f, 0x03};

const unsigned char PROGMEM smiley_face[] = {
    // width, height,
    16,   16,   0xe0, 0x18, 0x04, 0x02, 0x32, 0x31, 0x01, 0x01, 0x01, 0x01,
    0x31, 0x32, 0x02, 0x04, 0x18, 0xe0, 0x03, 0x0c, 0x11, 0x22, 0x24, 0x44,
    0x44, 0x44, 0x44, 0x44, 0x44, 0x24, 0x22, 0x11, 0x0c, 0x03,
};

// Global state variable
GameState gameState = STATE_SPLASH_SCREEN;

// --- MENU MANAGEMENT (for STATE_MAIN_MENU) ---
enum MenuOption {
  MENU_START_GAME,
  MENU_SETTINGS,
  MENU_SCORES,
  NUM_MENU_OPTIONS
};

// --- EEPROM HIGH SCORE STORAGE ---
// Addresses 400-405 (safe range, away from bootloader/Arduboy2 reserved areas)
const int EE_ADDR_MAGIC = 400;    // 2 bytes: 0xAD 0xBE = valid data marker
const int EE_ADDR_SCORE = 402;    // 2 bytes: best Score Mode score
const int EE_ADDR_SURVIVAL = 404; // 2 bytes: best Survival games survived

int bestScoreModeScore = 0;
int bestSurvivalScore = 0;

// --- DEBUG VARIABLES ---
bool DEBUG_MODE_ENABLED = false;
MiniGameState DEBUG_MINIGAME = GAME_HURDLES;

MenuOption currentMenuOption = MENU_START_GAME;
MiniGameState currentMiniGame =
    GAME_SIMON; // Initialized properly in doMainMenu

MiniGameState gamePlaylist[NUM_GAMES];
uint8_t playlistIndex = NUM_GAMES;

// --- HELPER FUNCTION DECLARATIONS ---
void doSplashScreen();
void doMainMenu();
void doSettings();
void doScores();
void doGameplay();
void doGameOver();
void doMakeItHotGame();
void doMarshmallowDropGame();
void doDuckHuntGame();
void doSpaceDodgeGame();
void doLockpickGame();
void doColorGridGame();
void doRedLightGreenLightGame();
void doHurdlesGame();
void doTransition();
void generatePlaylist(MiniGameState previousLastGame);

// --- LED MANAGEMENT ---
unsigned long ledTurnOnTime = 0;
bool isLedOn = false;
unsigned int currentLedDuration = 250;

void turnOnLED(uint8_t color, unsigned int duration) {
  if (color == COLOR_GREEN) {
    arduboy.setRGBled(0, 255, 0);
    // High, short beep for success
    sound.tone(1046, 100);
  } else if (color == COLOR_RED) {
    arduboy.setRGBled(255, 0, 0);
    // Low, longer beep for failure
    sound.tone(130, 250);
  }
  isLedOn = true;
  ledTurnOnTime = millis();
  currentLedDuration = duration;
}

void turnOffLED() {
  arduboy.setRGBled(0, 0, 0);
  isLedOn = false;
}

void generatePlaylist(MiniGameState previousLastGame) {
  // fill array
  for (uint8_t i = 0; i < NUM_GAMES; i++) {
    gamePlaylist[i] = (MiniGameState)i;
  }

  // Fisher-Yates shuffle
  for (uint8_t i = NUM_GAMES - 1; i > 0; i--) {
    uint8_t j = random(i + 1);
    MiniGameState temp = gamePlaylist[i];
    gamePlaylist[i] = gamePlaylist[j];
    gamePlaylist[j] = temp;
  }

  // Make sure first is not previousLastGame
  if (gamePlaylist[0] == previousLastGame) {
    uint8_t swapIdx = 1 + random(NUM_GAMES - 1);
    MiniGameState temp = gamePlaylist[0];
    gamePlaylist[0] = gamePlaylist[swapIdx];
    gamePlaylist[swapIdx] = temp;
  }
  playlistIndex = 0;
}

// --- ARDUINO SETUP ---
void setup() {
  arduboy.begin();
  arduboy.setFrameRate(30);
  arduboy.initRandomSeed();
  arduboy.audio.on();
  // Load high scores from EEPROM if valid
  if (EEPROM.read(EE_ADDR_MAGIC) == 0xAD &&
      EEPROM.read(EE_ADDR_MAGIC + 1) == 0xBE) {
    bestScoreModeScore =
        ((int)EEPROM.read(EE_ADDR_SCORE) << 8) | EEPROM.read(EE_ADDR_SCORE + 1);
    bestSurvivalScore = ((int)EEPROM.read(EE_ADDR_SURVIVAL) << 8) |
                        EEPROM.read(EE_ADDR_SURVIVAL + 1);
  }
}

// --- ARDUINO LOOP: THE STATE MACHINE CORE ---
void loop() {
  if (!arduboy.nextFrame()) {
    return;
  }

  arduboy.clear();
  arduboy.pollButtons();

  // Handle flashcart menu return (UP+DOWN for 2 seconds / 60 frames @ 30fps)
  if (arduboy.pressed(UP_BUTTON) && arduboy.pressed(DOWN_BUTTON)) {
    exitTimer++;
    if (exitTimer >= 60) {
      Arduboy2Core::exitToBootloader();
    }
  } else {
    exitTimer = 0;
  }

  // Check LED timeout
  if (isLedOn && (millis() - ledTurnOnTime >= currentLedDuration)) {
    turnOffLED();
  }

  // The main switch handles which scene's logic to execute
  switch (gameState) {
  case STATE_SPLASH_SCREEN:
    doSplashScreen();
    break;
  case STATE_MAIN_MENU:
    doMainMenu();
    break;
  case STATE_INTERMISSION:
    doIntermission();
    break;
  case STATE_TRANSITION:
    doTransition();
    break;
  case STATE_GAMEPLAY:
    doGameplay();
    break;
  case STATE_SETTINGS:
    doSettings();
    break;
  case STATE_GAME_OVER:
    doGameOver();
    break;
  case STATE_SCORES:
    doScores();
    break;
  default:
    break;
  }

  // Always display the buffer after the scene logic has finished drawing
  arduboy.display();
}

// --- MINI GAMES ---

// --- MAKE IT HOT GAME ---

// --- STATE IMPLEMENTATION FUNCTIONS ---

void doSplashScreen() {
  arduboy.drawCompressed(0, 0, arduware_title_compressed);

  // Transition logic for this state: wait for a button press
  if (arduboy.justPressed(A_BUTTON | B_BUTTON)) {
    // Re-seed the RNG with the exact millisecond the user pressed the button
    // I am not changing this yet, I need to check the build outputs instead.py.
    randomSeed(millis());
    gameState = STATE_MAIN_MENU;
    // Optionally reset menu cursor here if needed
    currentMenuOption = MENU_START_GAME;
  }
}

// Survival mode: max points per minigame (indexed by MiniGameState enum order)
static const uint8_t PROGMEM kMaxPts[] = {
    100, // GAME_ARROWS         (10 arrows x 10pts)
    50,  // GAME_SIMON          (5 buttons x 10pts, proportional on partial)
    120, // GAME_SPOTLIGHT      (10pts/500ms over 6100ms = 12 ticks max)
    40,  // GAME_MAKE_IT_HOT    (4 sections x 10pts)
    60,  // GAME_MARSHMALLOW    (6 marshmallows x 10pts)
    40,  // GAME_DUCK_HUNT      (4 ducks x 10pts)
    60,  // GAME_SPACE_DODGE    (starts at 60, -10 per asteroid hit)
    100, // GAME_LOCKPICK       (starts at 100, -10/sec)
    60,  // GAME_COLORGRID      (6 target squares x 10pts)
    100, // GAME_RED_LIGHT_GREEN_LIGHT (win=100; fail/timeout capped at 100)
    50,  // GAME_HURDLES        (10s game, +10 every 2s = 5 ticks max)
};
// Grace zone: earn >= 75% of max → no penalty; below 75% → scale from
// threshold. Applied to games where perfect play is unrealistic (Spotlight,
// Lockpick).
static const uint8_t PROGMEM kGrace[] = {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0};

void processMinigameEnd() {
  if (currentGameMode != MODE_SURVIVAL)
    return;

  uint8_t idx = (uint8_t)currentMiniGame;
  if (idx >= NUM_GAMES)
    return;

  int maxPoints = pgm_read_byte(&kMaxPts[idx]);
  if (maxPoints <= 0)
    return;

  int earned = minigamePointsEarned;
  if (earned < 0)
    earned = 0;
  if (earned > maxPoints)
    earned = maxPoints;

  int healthToDeduct;
  if (pgm_read_byte(&kGrace[idx])) {
    // No penalty above 75% of max; scale proportionally below that threshold
    int threshold = (maxPoints * 3) / 4;
    healthToDeduct =
        (earned >= threshold) ? 0 : ((threshold - earned) * 50) / threshold;
  } else {
    healthToDeduct = ((maxPoints - earned) * 50) / maxPoints;
  }

  playerHealth -= healthToDeduct;
  if (playerHealth < 0)
    playerHealth = 0;
}

int intermissionTime = 0;
void doIntermission() {
  turnOffLED();
  if (intermissionTime == 0) {
    intermissionTime = millis() / 1000;
    processMinigameEnd();
    if (currentGameMode == MODE_SURVIVAL && playerHealth <= 0) {
      intermissionTime = 0;
      gameState = STATE_GAME_OVER;
      return;
    }
    minigamesSurvived++;
  }
  arduboy.setCursor(25, MAX_Y_POS / 2 - 10);
  arduboy.print(F("Intermission"));
  switch (millis() / 1000 - intermissionTime) {
  case 0:
    arduboy.setCursor(MAX_X_POS / 2, MAX_Y_POS / 2);
    arduboy.print(F("3"));
    break;
  case 1:
    arduboy.setCursor(MAX_X_POS / 2, MAX_Y_POS / 2);
    arduboy.print(F("2"));
    break;
  case 2:
    arduboy.setCursor(MAX_X_POS / 2, MAX_Y_POS / 2);
    arduboy.print(F("1"));
    break;
  case 3:
    if (DEBUG_MODE_ENABLED) {
      currentMiniGame = DEBUG_MINIGAME;
    } else {
      if (playlistIndex >= NUM_GAMES) {
        if (currentGameMode == MODE_SCORE) {
          intermissionTime = 0;
          gameState = STATE_GAME_OVER;
          return;
        }
        generatePlaylist(currentMiniGame);
      }
      currentMiniGame = gamePlaylist[playlistIndex++];
    }
    intermissionTime = 0;
    gameState = STATE_TRANSITION;
    break;
  default:
    break;
  }
}

void doMainMenu() {
  arduboy.setCursor(40, 5);
  arduboy.print(F("MAIN MENU"));
  arduboy.drawFastHLine(0, 15, 128); // Separator

  // Handle menu navigation (UP/DOWN)
  if (arduboy.justPressed(UP_BUTTON)) {
    // Wrap around when moving up from the top option
    currentMenuOption =
        (MenuOption)((currentMenuOption - 1 + NUM_MENU_OPTIONS) %
                     NUM_MENU_OPTIONS);
  }
  if (arduboy.justPressed(DOWN_BUTTON)) {
    // Wrap around when moving down from the bottom option
    currentMenuOption =
        (MenuOption)((currentMenuOption + 1) % NUM_MENU_OPTIONS);
  }

  // Draw the menu options
  for (int i = 0; i < NUM_MENU_OPTIONS; i++) {
    arduboy.setCursor(45, 25 + (i * 10));

    // Indicate the currently selected option with an arrow
    if (i == currentMenuOption) {
      arduboy.print(">");
    } else {
      arduboy.print(" ");
    }

    switch (i) {
    case MENU_START_GAME:
      arduboy.print(F("START GAME"));
      break;
    case MENU_SETTINGS:
      arduboy.print(F("SETTINGS"));
      break;
    case MENU_SCORES:
      arduboy.print(F("SCORES"));
      break;
    }
  }

  // Handle selection (A/B button) and transition to the next state
  if (arduboy.justPressed(A_BUTTON | B_BUTTON)) {
    // Inject human entropy again
    randomSeed(random() + millis());

    switch (currentMenuOption) {
    case MENU_START_GAME:
      gameState = STATE_TRANSITION;
      score = 0;
      playerHealth = 100;
      minigamesSurvived = 0;
      // Game specific setup (e.g., reset player position, score = 0)
      if (DEBUG_MODE_ENABLED) {
        currentMiniGame = DEBUG_MINIGAME;
      } else {
        generatePlaylist(currentMiniGame);
        currentMiniGame = gamePlaylist[playlistIndex++];
      }
      break;
    case MENU_SETTINGS:
      gameState = STATE_SETTINGS;
      break;
    case MENU_SCORES:
      gameState = STATE_SCORES;
      break;
    }
  }
}

void doSettings() {
  arduboy.setCursor(40, 5);
  arduboy.print(F("SETTINGS"));
  arduboy.drawFastHLine(0, 15, 128);

  arduboy.setCursor(10, 25);
  arduboy.print(F("Vol:"));
  arduboy.print(arduboy.audio.enabled() ? F("ON") : F("OFF"));

  arduboy.setCursor(10, 38);
  arduboy.print(currentGameMode == MODE_SURVIVAL ? F("Mode:SURVIVAL")
                                                 : F("Mode:SCORE"));

  arduboy.setCursor(5, 54);
  arduboy.print(F("A:Vol B:Mode <- Back"));

  if (arduboy.justPressed(A_BUTTON)) {
    if (arduboy.audio.enabled())
      arduboy.audio.off();
    else
      arduboy.audio.on();
    arduboy.audio.saveOnOff();
  }
  if (arduboy.justPressed(B_BUTTON)) {
    currentGameMode =
        (currentGameMode == MODE_SURVIVAL) ? MODE_SCORE : MODE_SURVIVAL;
  }
  if (arduboy.justPressed(LEFT_BUTTON)) {
    gameState = STATE_MAIN_MENU;
  }
}

unsigned long transitionStartTime = 0;
void doTransition() {
  if (transitionStartTime == 0) {
    transitionStartTime = millis();
    minigamePointsEarned = 0; // Reset per-minigame tracker
  }

  // Space-efficient string table for minigame prompts
  static const char s0[] PROGMEM = "Buttons!";
  static const char s1[] PROGMEM = "Repeat!";
  static const char s2[] PROGMEM = "Chase!";
  static const char s3[] PROGMEM = "Smash!";
  static const char s4[] PROGMEM = "Catch!";
  static const char s5[] PROGMEM = "Hunt!";
  static const char s6[] PROGMEM = "Dodge!";
  static const char s7[] PROGMEM = "Pick!";
  static const char s8[] PROGMEM = "Colors!";
  static const char s9[] PROGMEM = "Run!";
  static const char s10[] PROGMEM = "Jump!";
  static const char sDefault[] PROGMEM = "Get Ready!";

  static const char *const stringTable[] PROGMEM = {s0, s1, s2, s3, s4, s5,
                                                    s6, s7, s8, s9, s10};

  char buffer[16];
  if (currentMiniGame < NUM_GAMES) {
    strcpy_P(buffer, (char *)pgm_read_word(&(stringTable[currentMiniGame])));
  } else {
    strcpy_P(buffer, sDefault);
  }

  uint8_t wordLength = strlen(buffer);
  uint8_t textWidth = wordLength * 6;
  arduboy.setCursor((MAX_X_POS - textWidth) / 2, MAX_Y_POS / 2 - 4);
  arduboy.print(buffer);

  // Wait for 2 seconds (2000 milliseconds)

  // Wait for 2 seconds (2000 milliseconds)
  if (millis() - transitionStartTime >= 2000) {
    transitionStartTime = 0;
    gameState = STATE_GAMEPLAY;
  }
}

void doGameplay() {
  if (currentGameMode == MODE_SCORE) {
    arduboy.setCursor(0, 0);
    arduboy.print(F("Score: "));
    arduboy.print(score);
  } else {
    // Health bar: outline box 52px wide, fill proportionally
    arduboy.drawRect(0, 0, 52, 6);
    uint8_t fillW = (uint8_t)((playerHealth * 50L) / 100);
    if (fillW > 0)
      arduboy.fillRect(1, 1, fillW, 4);
  }

  switch (currentMiniGame) {
  case GAME_ARROWS:
    doArrowsGame();
    break;
  case GAME_SIMON:
    doSimonGame();
    break;
  case GAME_SPOTLIGHT:
    doSpotlightGame();
    break;
  case GAME_MAKE_IT_HOT:
    doMakeItHotGame();
    break;
  case GAME_MARSHMALLOW_DROP:
    doMarshmallowDropGame();
    break;
  case GAME_DUCK_HUNT:
    doDuckHuntGame();
    break;
  case GAME_SPACE_DODGE:
    doSpaceDodgeGame();
    break;
  case GAME_LOCKPICK:
    doLockpickGame();
    break;
  case GAME_COLORGRID:
    doColorGridGame();
    break;
  case GAME_RED_LIGHT_GREEN_LIGHT:
    doRedLightGreenLightGame();
    break;
  case GAME_HURDLES:
    doHurdlesGame();
    break;
  default:
    break;
  }
}

void doGameOver() {
  // Save high scores on first frame (when just entered this state)
  static bool didSave = false;
  if (!didSave) {
    if (currentGameMode == MODE_SCORE && score > bestScoreModeScore) {
      bestScoreModeScore = score;
      EEPROM.update(EE_ADDR_MAGIC, 0xAD);
      EEPROM.update(EE_ADDR_MAGIC + 1, 0xBE);
      EEPROM.update(EE_ADDR_SCORE, (uint8_t)(bestScoreModeScore >> 8));
      EEPROM.update(EE_ADDR_SCORE + 1, (uint8_t)(bestScoreModeScore & 0xFF));
    }
    if (currentGameMode == MODE_SURVIVAL &&
        minigamesSurvived > bestSurvivalScore) {
      bestSurvivalScore = minigamesSurvived;
      EEPROM.update(EE_ADDR_MAGIC, 0xAD);
      EEPROM.update(EE_ADDR_MAGIC + 1, 0xBE);
      EEPROM.update(EE_ADDR_SURVIVAL, (uint8_t)(bestSurvivalScore >> 8));
      EEPROM.update(EE_ADDR_SURVIVAL + 1, (uint8_t)(bestSurvivalScore & 0xFF));
    }
    didSave = true;
  }

  arduboy.setCursor(13, 20);
  arduboy.print(F("G A M E   O V E R"));

  if (currentGameMode == MODE_SCORE) {
    arduboy.setCursor(23, 38);
    arduboy.print(F("Score: "));
    arduboy.print(score);
  } else {
    arduboy.setCursor(5, 38);
    arduboy.print(F("Games survived: "));
    arduboy.print(minigamesSurvived);
  }

  arduboy.setCursor(10, 54);
  arduboy.print(F("A: Menu"));

  if (arduboy.justPressed(A_BUTTON)) {
    didSave = false;
    gameState = STATE_MAIN_MENU;
    currentMenuOption = MENU_START_GAME;
    score = 0;
    playerHealth = 100;
    minigamesSurvived = 0;
  }
}

void doScores() {
  arduboy.setCursor(30, 5);
  arduboy.print(F("BEST SCORES"));
  arduboy.drawFastHLine(0, 15, 128);
  arduboy.setCursor(5, 22);
  arduboy.print(F("Score Mode: "));
  if (bestScoreModeScore > 0)
    arduboy.print(bestScoreModeScore);
  else
    arduboy.print(F("--"));
  arduboy.setCursor(5, 38);
  arduboy.print(F("Survival: "));
  if (bestSurvivalScore > 0)
    arduboy.print(bestSurvivalScore);
  else
    arduboy.print(F("--"));
  arduboy.setCursor(5, 54);
  arduboy.print(F("A: Back"));
  if (arduboy.justPressed(A_BUTTON | B_BUTTON))
    gameState = STATE_MAIN_MENU;
}
