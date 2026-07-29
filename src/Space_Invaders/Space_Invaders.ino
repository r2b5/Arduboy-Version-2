#include <Arduboy2.h>
Arduboy2 arduboy;

// DISPLAY SETTINGS

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Input settings
#define FIRE_BUT 7
#define RIGHT_BUT A1
#define LEFT_BUT A2

// Alien Settings
#define NUM_ALIEN_COLUMNS 7
#define NUM_ALIEN_ROWS 3
#define X_START_OFFSET 6
#define SPACE_BETWEEN_ALIEN_COLUMNS 5
#define LARGEST_ALIEN_WIDTH 11
#define SPACE_BETWEEN_ROWS 9
#define INVADERS_DROP_BY 4            // pixel amount that invaders move down by
#define INVADERS_SPEED 20             // speed of movement, lower=faster.
#define INVADER_HEIGHT 8

// Player settingsc
#define TANKGFX_WIDTH 13
#define TANKGFX_HEIGHT 8
#define PLAYER_X_MOVE_AMOUNT 1
#define PLAYER_Y_START 56
#define PLAYER_X_START 0

#define MISSILE_HEIGHT 4
#define MISSILE_WIDTH 1
#define MISSILE_SPEED 1

// Status of a game object constants
#define ACTIVE 0
#define DESTROYED 2

// graphics
// aliens

const unsigned char InvaderTopGfx [] PROGMEM = {
  0x98, 0x5c, 0xb6, 0x5f, 0x5f, 0xb6, 0x5c, 0x98
};

const unsigned char InvaderTopGfx2 [] PROGMEM = {
  0x58, 0xbc, 0x16, 0x1f, 0x1f, 0x16, 0xbc, 0x58
};

const unsigned char PROGMEM InvaderMiddleGfx [] =
{
  0x1e, 0xb8, 0x7d, 0x36, 0x3c, 0x3c, 0x3c, 0x36, 0x7d, 0xb8, 0x1e
};

const unsigned char PROGMEM InvaderMiddleGfx2 [] = {
  0x78, 0x18, 0x7d, 0xb6, 0xbc, 0x3c, 0xbc, 0xb6, 0x7d, 0x18, 0x78
};

const unsigned char PROGMEM InvaderBottomGfx [] = {
  0x1c, 0x5e, 0xfe, 0xb6, 0x37, 0x5f, 0x5f, 0x37, 0xb6, 0xfe, 0x5e, 0x1c
};

const unsigned char PROGMEM InvaderBottomGfx2 [] = {
  0x9c, 0xde, 0x7e, 0x36, 0x37, 0x5f, 0x5f, 0x37, 0x36, 0x7e, 0xde, 0x9c
};

// Player grafix
const unsigned char PROGMEM TankGfx [] = {
  0xf0, 0xf8, 0xf8, 0xf8, 0xf8, 0xfe, 0xff, 0xfe, 0xf8, 0xf8, 0xf8, 0xf8, 0xf0
};

static const unsigned char PROGMEM MissileGfx [] = {
  0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

};


// Game structures

struct GameObjectStruct  {
  // base object which most other objects will include
  signed int X;
  signed int Y;
  unsigned char Status;  //0 active, 1 exploding, 2 destroyed
};

struct AlienStruct  {
  GameObjectStruct Ord;
};

struct PlayerStruct  {
  GameObjectStruct Ord;
};

//alien global vars
//The array of aliens across the screen
AlienStruct  Alien[NUM_ALIEN_COLUMNS][NUM_ALIEN_ROWS];


// widths of aliens
// as aliens are the same type per row we do not need to store their graphic width per alien in the structure above
// that would take a byte per alien rather than just three entries here, 1 per row, saving significnt memory
byte AlienWidth[] = {8, 11, 12}; // top, middle ,bottom widths


char AlienXMoveAmount = 1; // norm is 2 , this is pixel movement in X
signed char InvadersMoveCounter;            // counts down, when 0 move invaders, set according to how many aliens on screen
bool AnimationFrame = false; // two frames of animation, if true show one if false show the other


// Player global variables
PlayerStruct Player;
GameObjectStruct Missile;


void setup()
{
  arduboy.begin();
  arduboy.setFrameRate(60);

  InitAliens(0);
  InitPlayer();

  pinMode(RIGHT_BUT, INPUT_PULLUP);
  pinMode(LEFT_BUT, INPUT_PULLUP);
  pinMode(FIRE_BUT, INPUT_PULLUP);

}

void loop()
{
  if (!arduboy.nextFrame()) {
    return;
  }

  Physics();
  UpdateDisplay();

}

void Physics()  {
  AlienControl();
  PlayerControl();
  MissileControl();
  CheckCollisions();
}


void PlayerControl()  {
  // user input checks
  if ((digitalRead(RIGHT_BUT) == 0) & (Player.Ord.X + TANKGFX_WIDTH < SCREEN_WIDTH))
    Player.Ord.X += PLAYER_X_MOVE_AMOUNT;
  if ((digitalRead(LEFT_BUT) == 0) & (Player.Ord.X > 0))
    Player.Ord.X -= PLAYER_X_MOVE_AMOUNT;
  if ((digitalRead(FIRE_BUT) == 0) & (Missile.Status != ACTIVE))
  {
    Missile.X = Player.Ord.X + (6); // offset missile so its in the mideel of the tank
    Missile.Y = PLAYER_Y_START;
    Missile.Status = ACTIVE;

  }
}

void MissileControl()
{
  if (Missile.Status == ACTIVE)
  {
    Missile.Y -= MISSILE_SPEED;
    if (Missile.Y + MISSILE_HEIGHT < 0)  // If off top of screen destroy so can be used again
      Missile.Status = DESTROYED;
  }
}


void AlienControl()
{
  if ((InvadersMoveCounter--) < 0)
  {
    bool Dropped = false;
    if ((RightMostPos() + AlienXMoveAmount >= SCREEN_WIDTH) | (LeftMostPos() + AlienXMoveAmount < 0)) // at edge of screen
    {
      AlienXMoveAmount = -AlienXMoveAmount;             // reverse direction
      Dropped = true;                                   // and indicate we are dropping
    }
    // update the alien postions
    for (int Across = 0; Across < NUM_ALIEN_COLUMNS; Across++)
    {
      for (int Down = 0; Down < 3; Down++)
      {
        if (Alien[Across][Down].Ord.Status == ACTIVE)
        {
          if (Dropped == false)
            Alien[Across][Down].Ord.X += AlienXMoveAmount;
          else
            Alien[Across][Down].Ord.Y += INVADERS_DROP_BY;
        }
      }
    }
    InvadersMoveCounter = INVADERS_SPEED;
    AnimationFrame = !AnimationFrame; ///swap to other frame
  }
}


void CheckCollisions()
{
  MissileAndAlienCollisions();
}


void MissileAndAlienCollisions()
{
  for (int across = 0; across < NUM_ALIEN_COLUMNS; across++)
  {
    for (int down = 0; down < NUM_ALIEN_ROWS; down++)
    {
      if (Alien[across][down].Ord.Status == ACTIVE)
      {
        if (Missile.Status == ACTIVE)
        {
          if (Collision(Missile, MISSILE_WIDTH, MISSILE_HEIGHT, Alien[across][down].Ord, AlienWidth[down], INVADER_HEIGHT))
          {
            // missile hit
            Alien[across][down].Ord.Status = DESTROYED;
            Missile.Status = DESTROYED;
          }
        }
      }
    }
  }
}

bool Collision(GameObjectStruct Obj1, unsigned char Width1, unsigned char Height1, GameObjectStruct Obj2, unsigned char Width2, unsigned char Height2)
{
  return ((Obj1.X + Width1 > Obj2.X) & (Obj1.X < Obj2.X + Width2) & (Obj1.Y + Height1 > Obj2.Y) & (Obj1.Y < Obj2.Y + Height2));
}

int RightMostPos()  {
  //returns x pos of right most alien
  int Across = NUM_ALIEN_COLUMNS - 1;
  int Down;
  int Largest = 0;
  int RightPos;
  while (Across >= 0) {
    Down = 0;
    while (Down < NUM_ALIEN_ROWS) {
      if (Alien[Across][Down].Ord.Status == ACTIVE)
      {
        // different aliens have different widths, add to x pos to get rightpos
        RightPos = Alien[Across][Down].Ord.X + AlienWidth[Down];
        if (RightPos > Largest)
          Largest = RightPos;
      }
      Down++;
    }
    if (Largest > 0) // we have found largest for this coloum
      return Largest;
    Across--;
  }
  return 0;  // should never get this far
}

int LeftMostPos()  {
  //returns x pos of left most alien
  int Across = 0;
  int Down;
  int Smallest = SCREEN_WIDTH * 2;
  while (Across < NUM_ALIEN_COLUMNS) {
    Down = 0;
    while (Down < 3) {
      if (Alien[Across][Down].Ord.Status == ACTIVE)
        if (Alien[Across][Down].Ord.X < Smallest)
          Smallest = Alien[Across][Down].Ord.X;
      Down++;
    }
    if (Smallest < SCREEN_WIDTH * 2) // we have found smalest for this coloum
      return Smallest;
    Across++;
  }
  return 0;  // should never get this far
}

void UpdateDisplay()
{
  arduboy.clear();
  for (int across = 0; across < NUM_ALIEN_COLUMNS; across++)
  {
    for (int down = 0; down < NUM_ALIEN_ROWS; down++)
    {
      if (Alien[across][down].Ord.Status == ACTIVE) {
        switch (down)  {
          case 0:
            if (AnimationFrame)
              arduboy.drawBitmap(Alien[across][down].Ord.X, Alien[across][down].Ord.Y,  InvaderTopGfx, AlienWidth[down], INVADER_HEIGHT, WHITE);
            else
              arduboy.drawBitmap(Alien[across][down].Ord.X, Alien[across][down].Ord.Y,  InvaderTopGfx2, AlienWidth[down], INVADER_HEIGHT, WHITE);
            break;
          case 1:
            if (AnimationFrame)
              arduboy.drawBitmap(Alien[across][down].Ord.X, Alien[across][down].Ord.Y,  InvaderMiddleGfx, AlienWidth[down], INVADER_HEIGHT, WHITE);
            else
              arduboy.drawBitmap(Alien[across][down].Ord.X, Alien[across][down].Ord.Y,  InvaderMiddleGfx2, AlienWidth[down], INVADER_HEIGHT, WHITE);
            break;
          default:
            if (AnimationFrame)
              arduboy.drawBitmap(Alien[across][down].Ord.X, Alien[across][down].Ord.Y,  InvaderBottomGfx, AlienWidth[down], INVADER_HEIGHT, WHITE);
            else
              arduboy.drawBitmap(Alien[across][down].Ord.X, Alien[across][down].Ord.Y,  InvaderBottomGfx2, AlienWidth[down], INVADER_HEIGHT, WHITE);
        }
      }
    }
  }

  // player
  arduboy.drawBitmap(Player.Ord.X, Player.Ord.Y,  TankGfx, TANKGFX_WIDTH, TANKGFX_HEIGHT, WHITE);
  //missile
  if (Missile.Status == ACTIVE)
    arduboy.drawBitmap(Missile.X, Missile.Y,  MissileGfx, MISSILE_WIDTH, MISSILE_HEIGHT, WHITE);

  arduboy.display();
}


void InitPlayer()  {
  Player.Ord.Y = PLAYER_Y_START;
  Player.Ord.X = PLAYER_X_START;
  Missile.Status = DESTROYED;
}

void InitAliens(int YStart)  {
  for (int across = 0; across < NUM_ALIEN_COLUMNS; across++)  {
    for (int down = 0; down < 3; down++)  {
      // we add down to centralise the aliens, just happens to be the right value we need per row!
      // we need to adjust a little as row zero should be 2, row 1 should be 1 and bottom row 0
      Alien[across][down].Ord.X = X_START_OFFSET + (across * (LARGEST_ALIEN_WIDTH + SPACE_BETWEEN_ALIEN_COLUMNS)) - (AlienWidth[down] / 2);
      Alien[across][down].Ord.Y = YStart + (down * SPACE_BETWEEN_ROWS);
    }
  }
}