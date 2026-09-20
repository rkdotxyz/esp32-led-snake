// =====================================================================
// snake_matrix.ino
// ---------------------------------------------------------------------
// The main sketch. It connects the modules together:
//
//   Serial keys --+
//                 +--> input --> snake_game --> drawGame() --> display
//   web_control --+                   |
//        ^                            |
//        +-------- sendStatus() <-----+   (score and state back to phones)
//
// Phase 5: play from any browser. Join the "ESP32-Snake" WiFi network,
// open http://snake.local, and steer with the on-screen pad, swipes or
// the keyboard. Serial keys (W A S D, R, P) still work too.
//
// Serial Monitor: 115200 baud.
// =====================================================================

#include "config.h"
#include "display.h"
#include "snake_game.h"
#include "input.h"
#include "web_control.h"

unsigned long lastTick = 0;       // when the snake last moved
bool paused = true;               // a new game waits for the first direction
bool started = false;             // has this game had its first move yet?
bool gameOverAnnounced = false;   // so game over is reported only once
int lastClientCount = 0;          // to notice controllers joining or leaving


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("snake_matrix: phase 5");

  displayBegin();
  webBegin();
  startNewGame();
}


void loop() {
  // 1. Housekeeping and inputs, on every pass.
  inputUpdate();       // Serial keys. Phone commands arrive on their own.
  webUpdate();
  watchControllers();

  if (inputRestartRequested()) {
    startNewGame();
  }

  // 2. Game over: report it once, then wait for a restart.
  if (gameIsOver()) {
    if (!gameOverAnnounced) {
      gameOverAnnounced = true;
      Serial.printf("Game over: %s. Score: %d\r\n", gameOverReason(), gameScore());
      drawGame();                    // redraw so the head shows red
      sendStatus();
    }
    return;
  }

  if (inputPauseRequested()) {
    setPaused(!paused);              // pause button toggles
  }

  // 3. Paused (or waiting to start): any direction gets things moving.
  if (paused) {
    Direction d;
    if (inputNextDirection(d)) {
      gameTurn(d);
      started = true;
      setPaused(false);
      lastTick = millis();
    }
    return;
  }

  // 4. Playing: move the snake once per tick.
  if (millis() - lastTick >= TICK_MS) {
    lastTick = millis();

    Direction d;
    if (inputNextDirection(d)) {     // one queued turn per step
      gameTurn(d);
    }

    int scoreBefore = gameScore();
    gameStep();
    if (gameScore() != scoreBefore) {
      sendStatus();                  // phones update their score
    }

    drawGame();
  }
}


void startNewGame() {
  inputClear();
  gameReset();
  paused = true;
  started = false;
  gameOverAnnounced = false;
  drawGame();
  sendStatus();
  Serial.println("New game. Press a direction to start.");
}


void setPaused(bool p) {
  paused = p;
  Serial.println(paused ? "Paused" : "Playing");
  sendStatus();
}


// Notices controllers connecting or disconnecting. If the last one
// disconnects mid-game (phone locked, walked away), the game pauses
// instead of letting the snake crash on its own.
void watchControllers() {
  int count = webClientCount();
  if (count == lastClientCount) {
    return;
  }

  if (count > lastClientCount) {
    Serial.printf("Controller connected (%d connected)\r\n", count);
    sendStatus();                    // the new controller needs the current score and state
  } else {
    Serial.printf("Controller disconnected (%d connected)\r\n", count);
    if (count == 0 && started && !paused && !gameIsOver()) {
      setPaused(true);
    }
  }
  lastClientCount = count;
}


// The game state as one word, for the controllers.
const char* stateName() {
  if (gameIsOver()) return "over";
  if (!started)     return "ready";
  if (paused)       return "paused";
  return "playing";
}


// Tells every controller the current score and state.
void sendStatus() {
  char message[64];   // room for the longest message we send

  // snprintf writes formatted text into `message`, never past its size.
  snprintf(message, sizeof(message), "score %d", gameScore());
  webSend(message);

  if (gameIsOver()) {
    snprintf(message, sizeof(message), "reason %s", gameOverReason());
    webSend(message);                // sent before "state over", so the page has it ready
  }

  snprintf(message, sizeof(message), "state %s", stateName());
  webSend(message);
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
