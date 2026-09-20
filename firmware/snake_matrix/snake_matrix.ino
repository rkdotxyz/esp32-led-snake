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
// Phase 8: snake and food colours chosen in Settings, plus effects: a
// pulsing head, a fading tail, pulsing food and an eat sparkle. The game
// now redraws on its own fast clock (FRAME_MS), separate from the
// snake's movement clock (TICK_MS), so effects stay smooth.
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
#include "theme.h"

AppState state = STATE_ATTRACT;
unsigned long stateStart = 0;     // when the current state began
unsigned long lastTick = 0;       // when the snake last moved
unsigned long lastFrame = 0;      // when an animation last redrew
int lastClientCount = 0;          // to notice controllers joining or leaving


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("snake_matrix: phase 8");

  // Restore the saved settings.
  settingsBegin();
  gameSetWallMode(settingsWallMode());
  themeSetSnakeColour(settingsSnakeColour());
  themeSetFoodColour(settingsFoodColour());
  Serial.printf("Edges: %s, snake #%06lx, food #%06lx\r\n", modeName(),
                (unsigned long)themeSnakeColour(), (unsigned long)themeFoodColour());

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

  handleSettingsRequests();

  // 2. Whatever the current state needs to do.
  unsigned long now = millis();
  switch (state) {
    case STATE_ATTRACT:   updateAttract(now);                 break;
    case STATE_PLAYING:   updatePlaying(now, pausePressed);   break;
    case STATE_PAUSED:    updatePaused(now, pausePressed);    break;
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


// Two clocks run here, independently:
//   TICK_MS  (250 ms): the snake moves one square.
//   FRAME_MS (30 ms):  the picture is redrawn, so effects animate smoothly
//                      even while the snake is standing still between steps.
void updatePlaying(unsigned long now, bool pausePressed) {
  if (pausePressed) {
    enterState(STATE_PAUSED);
    return;
  }

  if (now - lastTick >= TICK_MS) {
    lastTick = now;

    Direction d;
    if (inputNextDirection(d)) {     // one queued turn per step
      gameTurn(d);
    }

    int scoreBefore = gameScore();
    gameStep();

    if (gameIsOver()) {
      enterState(STATE_GAME_OVER);
      return;
    }
    if (gameScore() != scoreBefore) {
      Point head = gameSegment(0);   // the head is where the food was
      themeStartSparkle(head.x, head.y);
      sendStatus();                  // phones update their score
    }
  }

  drawIfDue(now);
}


void updatePaused(unsigned long now, bool pausePressed) {
  Direction d;
  if (inputNextDirection(d)) {       // a direction resumes and turns
    gameTurn(d);
    enterState(STATE_PLAYING);
    return;
  }
  if (pausePressed) {                // or the pause button again
    enterState(STATE_PLAYING);
    return;
  }
  drawIfDue(now);                    // keeps the head pulsing while paused
}


void drawIfDue(unsigned long now) {
  if (now - lastFrame >= FRAME_MS) {
    lastFrame = now;
    drawGame();
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


// ---------- Settings from the controller ----------

// Settings only change between games. The page's Settings button is
// disabled during a game too, but the ESP32 is the one that decides:
// a second phone, or an old cached page, could still ask.
void handleSettingsRequests() {
  WallMode mode;
  uint32_t snakeRgb;
  uint32_t foodRgb;
  bool wantsMode = inputModeRequested(mode);
  bool wantsSnake = inputSnakeColourRequested(snakeRgb);
  bool wantsFood = inputFoodColourRequested(foodRgb);

  if (!wantsMode && !wantsSnake && !wantsFood) {
    return;
  }

  if (state == STATE_PLAYING || state == STATE_PAUSED) {
    Serial.println("Settings can only change between games");
  } else {
    if (wantsMode)  applyWallMode(mode);
    if (wantsSnake) applySnakeColour(snakeRgb);
    if (wantsFood)  applyFoodColour(foodRgb);
  }
  sendStatus();                      // confirm the real settings to the phones either way
}


// Each apply function only writes to flash when the value really changes.

void applyWallMode(WallMode m) {
  if (m != gameWallMode()) {
    gameSetWallMode(m);
    settingsSaveWallMode(m);
    Serial.printf("Edges: %s\r\n", modeName());
  }
}


void applySnakeColour(uint32_t rgb) {
  rgb = themeNormalise(rgb);         // full brightness, same hue
  if (rgb != themeSnakeColour()) {
    themeSetSnakeColour(rgb);
    settingsSaveSnakeColour(rgb);
    Serial.printf("Snake colour: #%06lx\r\n", (unsigned long)rgb);
  }
}


void applyFoodColour(uint32_t rgb) {
  rgb = themeNormalise(rgb);
  if (rgb != themeFoodColour()) {
    themeSetFoodColour(rgb);
    settingsSaveFoodColour(rgb);
    Serial.printf("Food colour: #%06lx\r\n", (unsigned long)rgb);
  }
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

  // %06lx = hex, at least 6 digits, padded with zeros: 0x00FF40 -> "00ff40"
  snprintf(message, sizeof(message), "snake #%06lx", (unsigned long)themeSnakeColour());
  webSend(message);
  snprintf(message, sizeof(message), "food #%06lx", (unsigned long)themeFoodColour());
  webSend(message);

  if (state == STATE_GAME_OVER || state == STATE_SCORE) {
    snprintf(message, sizeof(message), "reason %s", gameOverReason());
    webSend(message);                // before "state over", so the page has it ready
  }

  snprintf(message, sizeof(message), "state %s", stateName());
  webSend(message);
}


// ---------- Drawing the game itself ----------

// Layers, back to front: food, sparkle, body, head. Later layers are
// drawn over earlier ones, so the snake always sits on top.
void drawGame() {
  displayClear();

  Point food = gameFood();
  displaySetPixel(food.x, food.y, themeFood());

  themeDrawSparkle();

  int length = gameLength();
  for (int i = length - 1; i >= 1; i--) {
    Point p = gameSegment(i);
    displaySetPixel(p.x, p.y, themeBody(i, length));
  }
  Point head = gameSegment(0);
  displaySetPixel(head.x, head.y, themeHead());

  displayShow();
}
