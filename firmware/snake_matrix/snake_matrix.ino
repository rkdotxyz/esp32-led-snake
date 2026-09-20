// =====================================================================
// snake_matrix.ino
// ---------------------------------------------------------------------
// The main sketch. It doesn't contain the game's rules or any LED
// details itself; it connects the modules together:
//
//   input  ->  snake_game  ->  drawGame()  ->  display
//
// Phase 4: playable Snake, steered from the keyboard over USB.
//   W A S D = up, left, down, right      R = restart
//
// Serial: 115200 baud.
// =====================================================================

#include "config.h"
#include "display.h"
#include "snake_game.h"
#include "input.h"

unsigned long lastTick = 0;       // when the snake last moved
bool gameOverAnnounced = false;   // so "Game over" prints only once


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("snake_matrix: phase 4");
  Serial.println("Keys: W A S D to steer, R to restart");

  displayBegin();
  startNewGame();
}


void loop() {
  // 1. Read keys on every pass, so no keypress is ever missed,
  //    even between snake steps.
  inputUpdate();

  if (inputRestartRequested()) {
    startNewGame();
  }

  // 2. Nothing else to do once the game has ended; wait for R.
  if (gameIsOver()) {
    if (!gameOverAnnounced) {
      // \r\n (carriage return + new line) keeps lines tidy in any terminal
      // program, not just Serial Monitor. Serial.println() adds it for you.
      Serial.printf("Game over: %s. Score: %d. Press R to play again.\r\n",
                    gameOverReason(), gameScore());
      gameOverAnnounced = true;
      drawGame();                  // redraw so the head shows red
    }
    return;
  }

  // 3. Move the snake once per tick.
  if (millis() - lastTick >= TICK_MS) {
    lastTick = millis();

    Direction d;
    if (inputNextDirection(d)) {   // one queued turn per step
      gameTurn(d);
    }

    int scoreBefore = gameScore();
    gameStep();
    if (gameScore() != scoreBefore) {
      Serial.printf("Score: %d\r\n", gameScore());
    }

    drawGame();
  }
}


void startNewGame() {
  inputClear();
  gameReset();
  gameOverAnnounced = false;
  lastTick = millis();
  drawGame();
  Serial.println("New game. Go!");
}


// Reads the game's state and draws it. The only place that decides
// what the game looks like; later phases add effects here.
void drawGame() {
  displayClear();

  Point food = gameFood();
  displaySetPixel(food.x, food.y, FOOD_COLOUR);

  // Body first (tail to neck), then the head on top.
  for (int i = gameLength() - 1; i >= 1; i--) {
    Point p = gameSegment(i);
    displaySetPixel(p.x, p.y, BODY_COLOUR);
  }
  Point head = gameSegment(0);
  displaySetPixel(head.x, head.y, gameIsOver() ? DEAD_COLOUR : HEAD_COLOUR);

  displayShow();
}
