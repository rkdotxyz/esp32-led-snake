// =====================================================================
// snake_game.cpp
// ---------------------------------------------------------------------
// How the Snake rules work.
//
// The snake is stored as a list of points, head first:
//   body[0] = head, body[1] = the segment behind it, ... up to the tail.
// Each step, every segment moves into the spot of the one in front of
// it, and the head moves one square in the current direction.
// =====================================================================

#include "snake_game.h"

// ---------- Game state ----------
// `static` keeps these private to this file. Other files can only read
// them through the functions at the bottom, never change them directly.

static Point body[NUM_LEDS];      // room for a snake that fills the whole board
static int length = 0;
static Direction dir = DIR_RIGHT;
static Point food;
static int score = 0;
static bool over = false;
static const char* reason = "";
static WallMode wallMode = MODE_WALLS;


// ---------- Helpers (private to this file) ----------

// Is the point p covered by the first `count` segments of the snake?
static bool isOnSnake(Point p, int count) {
  for (int i = 0; i < count; i++) {
    if (body[i].x == p.x && body[i].y == p.y) {
      return true;
    }
  }
  return false;
}


// Puts food on a random empty square. Returns false if there's no empty
// square left, which means the snake has filled the board.
//
// Instead of guessing random squares until one is free, it counts the
// free squares, picks one of them at random, then walks the board to
// find it. That always takes the same time, even when the board is
// nearly full.
static bool placeFood() {
  int freeCount = NUM_LEDS - length;
  if (freeCount <= 0) {
    return false;
  }

  // random(n) gives 0 to n-1. On the ESP32 it uses the chip's hardware
  // random number generator, so there's no need to "seed" it.
  int pick = random(freeCount);

  for (int y = 0; y < MATRIX_HEIGHT; y++) {
    for (int x = 0; x < MATRIX_WIDTH; x++) {
      Point p = {(int8_t)x, (int8_t)y};
      if (!isOnSnake(p, length)) {
        if (pick == 0) {
          food = p;
          return true;
        }
        pick--;
      }
    }
  }
  return false;   // not reached, but keeps the compiler happy
}


// ---------- Controlling the game ----------

void gameReset() {
  // Start in the middle-left, pointing right, with the tail behind.
  length = START_LENGTH;
  int startX = 6;
  int startY = MATRIX_HEIGHT / 2;
  for (int i = 0; i < length; i++) {
    body[i] = {(int8_t)(startX - i), (int8_t)startY};
  }
  dir = DIR_RIGHT;
  score = 0;
  over = false;
  reason = "";
  placeFood();
}


void gameTurn(Direction d) {
  // Turning straight back would drive the head into the neck, so ignore it.
  bool reversing =
      (d == DIR_UP    && dir == DIR_DOWN)  ||
      (d == DIR_DOWN  && dir == DIR_UP)    ||
      (d == DIR_LEFT  && dir == DIR_RIGHT) ||
      (d == DIR_RIGHT && dir == DIR_LEFT);
  if (!reversing) {
    dir = d;
  }
}


void gameStep() {
  if (over) {
    return;
  }

  // 1. Work out where the head is going.
  Point next = body[0];
  if (dir == DIR_UP)    next.y--;    // y counts downward, so up is minus
  if (dir == DIR_DOWN)  next.y++;
  if (dir == DIR_LEFT)  next.x--;
  if (dir == DIR_RIGHT) next.x++;

  // 2. Off the edge? Depends on the mode.
  bool offBoard = next.x < 0 || next.x >= MATRIX_WIDTH ||
                  next.y < 0 || next.y >= MATRIX_HEIGHT;
  if (offBoard) {
    if (wallMode == MODE_WALLS) {
      over = true;
      reason = "hit a wall";
      return;
    }
    // Wrap: % (remainder) folds the position back onto the board.
    //   x = 32 -> 32 % 32 = 0   (off the right, back in on the left)
    //   x = -1 -> (-1 + 32) % 32 = 31
    // Adding the width first keeps the number positive, because in C++
    // -1 % 32 is -1, not 31.
    next.x = (int8_t)((next.x + MATRIX_WIDTH) % MATRIX_WIDTH);
    next.y = (int8_t)((next.y + MATRIX_HEIGHT) % MATRIX_HEIGHT);
  }

  // 3. Eating this step?
  bool eating = (next.x == food.x && next.y == food.y);

  // 4. Hit itself? When not eating, the tail tip moves out of the way this
  //    same step, so the head may safely move into it. That's why we check
  //    one segment fewer when not eating.
  int segmentsToCheck = eating ? length : length - 1;
  if (isOnSnake(next, segmentsToCheck)) {
    over = true;
    reason = "hit itself";
    return;
  }

  // 5. Move. Growing just means keeping the tail: we make the snake one
  //    longer first, so the old tail gets copied into the new last slot.
  if (eating) {
    length++;
  }
  for (int i = length - 1; i > 0; i--) {
    body[i] = body[i - 1];          // each segment takes the place of the one ahead
  }
  body[0] = next;                   // the head moves forward

  // 6. Eaten? Score, and put new food down.
  if (eating) {
    score++;
    if (!placeFood()) {
      over = true;
      reason = "filled the whole board. You win!";
    }
  }
}


void gameSetWallMode(WallMode m) {
  wallMode = m;
}


// ---------- Reading the game ----------

int gameLength()            { return length; }
Point gameSegment(int i)    { return body[i]; }
Point gameFood()            { return food; }
int gameScore()             { return score; }
bool gameIsOver()           { return over; }
const char* gameOverReason() { return reason; }
WallMode gameWallMode()     { return wallMode; }
