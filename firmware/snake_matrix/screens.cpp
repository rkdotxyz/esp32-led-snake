// =====================================================================
// screens.cpp
// =====================================================================

#include "screens.h"
#include "config.h"
#include "display.h"
#include "snake_game.h"
#include "theme.h"


// =====================================================================
// Attract screen
// ---------------------------------------------------------------------
// A demo snake glides in a zigzag across an otherwise dark board,
// eating apples placed along its path and growing, then starts over.
// It uses the chosen theme, so it previews colour changes straight away.
//
// Its route is a loop that visits every square exactly once:
//   - rows 0-7 zigzag across columns 1-31 (right, left, right...)
//   - column 0 is the way back up from the bottom-left to the top.
// This works because there's an even number of rows, so the last row
// ends on the left, right next to column 0.
// =====================================================================

const int LOOP_LENGTH = NUM_LEDS;          // 256 squares, all visited once
const int ATTRACT_START_LENGTH = 5;
const int APPLE_SPACING = 23;              // an apple every 23 squares along the route


// Square number i along the loop -> its (x, y) position.
static void loopPoint(int i, int &x, int &y) {
  const int zigzagWidth = MATRIX_WIDTH - 1;              // columns 1-31
  const int zigzagSquares = zigzagWidth * MATRIX_HEIGHT; // 248

  if (i < zigzagSquares) {
    int row = i / zigzagWidth;
    int along = i % zigzagWidth;
    y = row;
    x = (row % 2 == 0) ? 1 + along           // even rows go right
                       : zigzagWidth - along; // odd rows come back left
  } else {
    // The last 8 squares: up column 0, from the bottom to the top.
    x = 0;
    y = (MATRIX_HEIGHT - 1) - (i - zigzagSquares);
  }
}


void screensDrawAttract(unsigned long elapsed) {
  // Everything follows from how many steps have passed. No history is
  // stored, which keeps this function simple.
  long step = elapsed / ATTRACT_STEP_MS;
  int head = step % LOOP_LENGTH;               // head's square on the loop this lap
  int eaten = head / APPLE_SPACING;            // apples passed so far this lap
  int length = ATTRACT_START_LENGTH + eaten;   // one segment longer per apple

  displayClear();

  // The next apple, if there's one left this lap.
  int apple = (eaten + 1) * APPLE_SPACING;
  if (apple < LOOP_LENGTH) {
    int ax, ay;
    loopPoint(apple, ax, ay);
    displaySetPixel(ax, ay, themeFood());
  }

  // Tail first, head last so it's drawn on top. Segments behind the head
  // are further back along the loop, wrapping round past square 0.
  for (int i = length - 1; i >= 0; i--) {
    int square = head - i;
    if (square < 0) {
      square += LOOP_LENGTH;
    }
    int x, y;
    loopPoint(square, x, y);

    if (i == 0) {
      displaySetPixel(x, y, themeHead());
    } else {
      displaySetPixel(x, y, themeBody(i, length));
    }
  }

  displayShow();
}


// =====================================================================
// Game-over animation
// ---------------------------------------------------------------------
// 1. The dead snake flashes red three times.
// 2. It crumbles away from the tail to the head.
// =====================================================================

const unsigned long FLASH_MS = 150;                // on or off for this long
const int FLASHES = 3;
const unsigned long FLASH_TOTAL_MS = FLASH_MS * 2 * FLASHES;
const unsigned long CRUMBLE_TOTAL_MS = 1200;       // roughly, for any length


static void drawSnake(int segments, CRGB colour) {
  for (int i = 0; i < segments; i++) {
    Point p = gameSegment(i);
    displaySetPixel(p.x, p.y, colour);
  }
}


bool screensDrawGameOver(unsigned long elapsed) {
  int length = gameLength();
  displayClear();

  // Part 1: flash. Even-numbered 150 ms slots are on, odd ones off.
  if (elapsed < FLASH_TOTAL_MS) {
    bool on = (elapsed / FLASH_MS) % 2 == 0;
    if (on) {
      drawSnake(length, DEAD_COLOUR);
    }
    displayShow();
    return false;
  }

  // Part 2: crumble. Remove one segment per step; long snakes crumble
  // faster per segment, so the whole thing takes about the same time.
  unsigned long stepMs = CRUMBLE_TOTAL_MS / length;
  if (stepMs < 20) {
    stepMs = 20;
  }
  int removed = (elapsed - FLASH_TOTAL_MS) / stepMs;
  int remaining = length - removed;

  if (remaining <= 0) {
    displayShow();       // empty board
    return true;         // finished: time for the score
  }
  drawSnake(remaining, DEAD_COLOUR);   // segment 0 is the head, so the tail goes first
  displayShow();
  return false;
}


// =====================================================================
// Final score
// ---------------------------------------------------------------------
// Digits in a 5 x 6 pixel font, counting up quickly to the final score.
// 6 rows tall leaves the top and bottom rows of the matrix empty, so the
// number sits centred with a border of dark space.
// Each digit is 6 rows; in each row, the 5 lowest bits are its pixels,
// left to right. 0b01110 means "off on on on off": the top of a 0.
// =====================================================================

static const uint8_t DIGITS[10][6] = {
  {0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110},  // 0
  {0b00100, 0b01100, 0b00100, 0b00100, 0b00100, 0b01110},  // 1
  {0b01110, 0b10001, 0b00010, 0b00100, 0b01000, 0b11111},  // 2
  {0b11110, 0b00001, 0b00110, 0b00001, 0b00001, 0b11110},  // 3
  {0b00010, 0b00110, 0b01010, 0b10010, 0b11111, 0b00010},  // 4
  {0b11111, 0b10000, 0b11110, 0b00001, 0b00001, 0b11110},  // 5
  {0b01110, 0b10000, 0b11110, 0b10001, 0b10001, 0b01110},  // 6
  {0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b01000},  // 7
  {0b01110, 0b10001, 0b01110, 0b10001, 0b10001, 0b01110},  // 8
  {0b01110, 0b10001, 0b10001, 0b01111, 0b00001, 0b01110},  // 9
};
const int DIGIT_WIDTH = 5;
const int DIGIT_HEIGHT = 6;
const int DIGIT_GAP = 1;
const unsigned long COUNT_UP_MS = 800;


static void drawDigit(int digit, int left, int top, CRGB colour) {
  for (int row = 0; row < DIGIT_HEIGHT; row++) {
    uint8_t bits = DIGITS[digit][row];
    for (int col = 0; col < DIGIT_WIDTH; col++) {
      // Shift the wanted bit down to position 0, then test it.
      // Column 0 is the leftmost pixel, which is the highest bit (bit 4).
      if ((bits >> (DIGIT_WIDTH - 1 - col)) & 1) {
        displaySetPixel(left + col, top + row, colour);
      }
    }
  }
}


void screensDrawScore(int score, unsigned long elapsed) {
  // Count up from 0 during the first COUNT_UP_MS.
  int shown = score;
  if (elapsed < COUNT_UP_MS) {
    shown = (int)((long)score * elapsed / COUNT_UP_MS);
  }

  // Lay out using the FINAL score's width, so the digits don't jump
  // sideways when the count goes from one digit to two. While counting
  // up, pad with leading zeros to that width: a final score of 12 counts
  // 00, 01 ... 12, so the number always fills the same centred space.
  char finalText[6];
  int digitCount = snprintf(finalText, sizeof(finalText), "%d", score);
  char text[6];
  snprintf(text, sizeof(text), "%0*d", digitCount, shown);  // %0*d = zero-padded to digitCount

  int width = digitCount * (DIGIT_WIDTH + DIGIT_GAP) - DIGIT_GAP;
  int left = (MATRIX_WIDTH - width) / 2;
  int top = (MATRIX_HEIGHT - DIGIT_HEIGHT) / 2;              // 1 on an 8-row matrix: rows 1-6

  displayClear();
  for (int i = 0; i < digitCount; i++) {
    drawDigit(text[i] - '0', left + i * (DIGIT_WIDTH + DIGIT_GAP), top, SCORE_COLOUR);
  }
  displayShow();
}
