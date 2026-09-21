// =====================================================================
// snake_game.h
// ---------------------------------------------------------------------
// The rules of Snake, and nothing else. This module never touches LEDs,
// WiFi or Serial: it only knows about a grid, a snake, some food and a
// direction. That separation is why effects, themes and the phone
// controller could all be added without changing these rules.
// =====================================================================

#pragma once

#include "config.h"


// The four directions the snake can move. An enum gives names to a
// fixed set of choices, so code reads DIR_UP instead of a mystery 0.
enum Direction {
  DIR_UP,
  DIR_DOWN,
  DIR_LEFT,
  DIR_RIGHT
};


// What happens at the edges of the board.
enum WallMode {
  MODE_WALLS,   // the edges are walls: touching one ends the game
  MODE_WRAP     // leave one edge, come back in at the opposite one
};


// A position on the grid. int8_t is a small whole number (-128 to 127),
// plenty for a 32 x 8 board, and it keeps the snake's memory small.
struct Point {
  int8_t x;
  int8_t y;
};


// ---------- Controlling the game ----------
void gameReset();                 // start a new game
void gameTurn(Direction d);       // request a turn; reversing into yourself is ignored
void gameStep();                  // move the snake one square; call once per tick
void gameSetWallMode(WallMode m); // walls or wrap; takes effect from the next step


// ---------- Reading the game (for drawing and messages) ----------
int gameLength();                 // how many segments the snake has
Point gameSegment(int i);         // segment i, where 0 is the head
Point gameFood();                 // where the food is
int gameScore();                  // how many pieces of food eaten
bool gameIsOver();                // true once the snake has died
const char* gameOverReason();     // why the game ended, as text
WallMode gameWallMode();          // the current edge rule
