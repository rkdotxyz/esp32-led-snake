// =====================================================================
// snake_matrix.ino
// ---------------------------------------------------------------------
// The main sketch: a state machine (see app_state.h) that decides which
// screen is showing and connects the modules:
//
//   Serial keys --+
//                 +--> input --> snake_game --> drawGame() --> display
//   web_control --+                                  screens --^
//
// Phase 7: wall and wrap-around modes, chosen in the controller's
// Settings between games, and remembered after power-off.
//
// Serial Monitor: 115200 baud. Serial keys W A S D, R, P still work.
// =====================================================================

#include "config.h"
#include "app_state.h"
#include "display.h"
#include "snake_game.h"
#include "input.h"
#include "web_control.h"
#include "screens.h"
#include "settings.h"

AppState state = STATE_ATTRACT;
unsigned long stateStart = 0;     // when the current state began
unsigned long lastTick = 0;       // when the snake last moved
unsigned long lastFrame = 0;      // when an animation last redrew
int lastClientCount = 0;          // to notice controllers joining or leaving


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("snake_matrix: phase 7");

  settingsBegin();
  gameSetWallMode(settingsWallMode());   // restore the last mode used
  Serial.printf("Edges: %s\r\n", modeName());

  displayBegin();
  webBegin();
  enterState(STATE_ATTRACT);
}


void loop() {
  // 1. Inputs and housekeeping, on every pass.
  inputUpdate();
  webUpdate();
  watchControllers();

  // Read the one-off requests once per loop, so a stray press in the
  // wrong state is used up rather than surprising us later.
  bool restartPressed = inputRestartRequested();
  bool pausePressed = inputPauseRequested();

  if (restartPressed) {
    enterState(STATE_ATTRACT);
    return;
  }

  // Mode changes are only allowed between games. The page's Settings
  // button is disabled during a game too, but the ESP32 is the one that
  // decides: a second phone, or an old cached page, could still ask.
  WallMode requestedMode;
  if (inputModeRequested(requestedMode)) {
    if (state == STATE_PLAYING || state == STATE_PAUSED) {
      Serial.println("Edges can only change between games");
      sendStatus();                  // re-confirm the current mode to the phones
    } else {
      applyWallMode(requestedMode);
    }
  }

  // 2. Whatever the current state needs to do.
  unsigned long now = millis();
  switch (state) {
    case STATE_ATTRACT:   updateAttract(now);                 break;
    case STATE_PLAYING:   updatePlaying(now, pausePressed);   break;
    case STATE_PAUSED:    updatePaused(pausePressed);         break;
    case STATE_GAME_OVER: updateGameOver(now);                break;
    case STATE_SCORE:     updateScore(now);                   break;
  }
}


// ---------- One function per state ----------

void updateAttract(unsigned long now) {
  Direction d;
  if (inputNextDirection(d)) {       // any direction starts a game
    startGame(d);
    return;
  }
  if (now - lastFrame >= FRAME_MS) {
    lastFrame = now;
    screensDrawAttract(now - stateStart);
  }
}


void updatePlaying(unsigned long now, bool pausePressed) {
  if (pausePressed) {
    enterState(STATE_PAUSED);
    return;
  }
  if (now - lastTick < TICK_MS) {
    return;                          // not time to move yet
  }
  lastTick = now;

  Direction d;
  if (inputNextDirection(d)) {       // one queued turn per step
    gameTurn(d);
  }

  int scoreBefore = gameScore();
  gameStep();

  if (gameIsOver()) {
    enterState(STATE_GAME_OVER);
    return;
  }
  if (gameScore() != scoreBefore) {
    sendStatus();                    // phones update their score
  }
  drawGame();
}


void updatePaused(bool pausePressed) {
  Direction d;
  if (inputNextDirection(d)) {       // a direction resumes and turns
    gameTurn(d);
    enterState(STATE_PLAYING);
  } else if (pausePressed) {         // or the pause button again
    enterState(STATE_PLAYING);
  }
}


void updateGameOver(unsigned long now) {
  if (now - lastFrame >= FRAME_MS) {
    lastFrame = now;
    bool finished = screensDrawGameOver(now - stateStart);
    if (finished) {
      enterState(STATE_SCORE);
    }
  }
}


void updateScore(unsigned long now) {
  Direction d;
  if (inputNextDirection(d)) {       // play again straight away
    startGame(d);
    return;
  }
  if (now - stateStart >= SCORE_SHOW_MS) {
    enterState(STATE_ATTRACT);
    return;
  }
  if (now - lastFrame >= FRAME_MS) {
    lastFrame = now;
    screensDrawScore(gameScore(), now - stateStart);
  }
}


// ---------- Moving between states ----------

// The single place state changes happen, so every change is logged and
// every controller hears about it.
void enterState(AppState next) {
  state = next;
  stateStart = millis();
  lastFrame = 0;                     // animations draw their first frame immediately

  switch (next) {
    case STATE_ATTRACT:
      inputClear();
      Serial.println("Ready. Press a direction to start.");
      break;
    case STATE_PLAYING:
      lastTick = millis();
      drawGame();
      Serial.println("Playing");
      break;
    case STATE_PAUSED:
      Serial.println("Paused");
      break;
    case STATE_GAME_OVER:
      Serial.printf("Game over: %s. Score: %d\r\n", gameOverReason(), gameScore());
      break;
    case STATE_SCORE:
      inputClear();                  // ignore panicked presses during the crash animation
      break;
  }

  sendStatus();
}


void startGame(Direction firstMove) {
  gameReset();
  gameTurn(firstMove);
  enterState(STATE_PLAYING);
}


// Switches walls/wrap, saves it, and tells every controller.
void applyWallMode(WallMode m) {
  if (m != gameWallMode()) {         // only write to flash when it really changes
    gameSetWallMode(m);
    settingsSaveWallMode(m);
    Serial.printf("Edges: %s\r\n", modeName());
  }
  sendStatus();                      // confirm to the phones either way
}


// If the last controller disconnects mid-game (phone locked, walked
// away), pause instead of letting the snake crash on its own.
void watchControllers() {
  int count = webClientCount();
  if (count == lastClientCount) {
    return;
  }

  if (count > lastClientCount) {
    Serial.printf("Controller connected (%d connected)\r\n", count);
    sendStatus();                    // the newcomer needs the current score and state
  } else {
    Serial.printf("Controller disconnected (%d connected)\r\n", count);
    if (count == 0 && state == STATE_PLAYING) {
      enterState(STATE_PAUSED);
    }
  }
  lastClientCount = count;
}


// ---------- Talking to the controllers ----------

// The state as the phone page understands it. The page doesn't need to
// know about the animation states, so both count as "over".
const char* stateName() {
  switch (state) {
    case STATE_ATTRACT: return "ready";
    case STATE_PLAYING: return "playing";
    case STATE_PAUSED:  return "paused";
    default:            return "over";
  }
}


const char* modeName() {
  return gameWallMode() == MODE_WRAP ? "wrap" : "walls";
}


void sendStatus() {
  char message[64];

  snprintf(message, sizeof(message), "score %d", gameScore());
  webSend(message);

  snprintf(message, sizeof(message), "mode %s", modeName());
  webSend(message);

  if (state == STATE_GAME_OVER || state == STATE_SCORE) {
    snprintf(message, sizeof(message), "reason %s", gameOverReason());
    webSend(message);                // before "state over", so the page has it ready
  }

  snprintf(message, sizeof(message), "state %s", stateName());
  webSend(message);
}


// ---------- Drawing the game itself ----------

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
  displaySetPixel(head.x, head.y, HEAD_COLOUR);

  displayShow();
}
