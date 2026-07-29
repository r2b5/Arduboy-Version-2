#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduboy2.h>
#include <ArduboyTones.h>

// Extern declarations for global instances
extern Arduboy2 arduboy;
extern ArduboyTones sound;

// Extern declarations for global game state
enum GameMode { MODE_SURVIVAL, MODE_SCORE };
extern GameMode currentGameMode;
extern int score;
extern int playerHealth;
extern int minigamesSurvived;
extern int minigamePointsEarned;
extern void addScore(int points);

const int MAX_X_POS = 128;
const int MAX_Y_POS = 64;

// --- STATE DEFINITIONS ---
enum GameState {
  STATE_SPLASH_SCREEN, // Initial state
  STATE_MAIN_MENU,     // Where the user selects an option
  STATE_INTERMISSION,
  STATE_TRANSITION, // 2-second word transition screen
  STATE_GAMEPLAY,   // The main action screen
  STATE_SETTINGS,   // A simple options screen
  STATE_GAME_OVER,  // End screen
  STATE_SCORES,     // High score screen
  NUM_GAME_STATES   // Total count of states (must be last)
};

extern GameState gameState;

enum MiniGameState {
  GAME_ARROWS,
  GAME_SIMON,
  GAME_SPOTLIGHT,
  GAME_MAKE_IT_HOT,
  GAME_MARSHMALLOW_DROP,
  GAME_DUCK_HUNT,
  GAME_SPACE_DODGE,
  GAME_LOCKPICK,
  GAME_COLORGRID,
  GAME_RED_LIGHT_GREEN_LIGHT,
  GAME_HURDLES,
  NUM_GAMES
};

extern bool DEBUG_MODE_ENABLED;
extern MiniGameState DEBUG_MINIGAME;

extern MiniGameState currentMiniGame;

// --- LED MANAGEMENT CONSTANTS ---
const uint8_t COLOR_GREEN = 0;
const uint8_t COLOR_RED = 1;
extern void turnOnLED(uint8_t color, unsigned int duration = 250);
extern void turnOffLED();

// --- BITMAPS ---
extern const unsigned char PROGMEM background[];
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
extern const uint8_t PROGMEM duck_right[];
extern const uint8_t PROGMEM duck_left[];
extern const uint8_t PROGMEM crosshairs[];
extern const uint8_t PROGMEM spaceship[];
extern const uint8_t PROGMEM asteroid_4[];
extern const uint8_t PROGMEM asteroid_8[];
extern const uint8_t PROGMEM asteroid_12[];
extern const uint8_t PROGMEM asteroid_16[];
extern const unsigned char PROGMEM smiley_face[];

#endif
