// =====================================================================
// input.cpp
// ---------------------------------------------------------------------
// Why a queue? The snake only moves once per tick. If you press "up"
// then "left" quickly within one tick, a single "latest key" variable
// would lose the "up". A queue keeps both, used one per tick, in order.
//
// New in phase 5: two tasks share this queue.
// The web server runs in its own background task (the ESP32 runs
// several tasks at once, even on two processor cores). So a phone can
// add a direction at the exact moment loop() is taking one out. If both
// touched the queue at once, it could be corrupted. A "critical
// section" (portENTER_CRITICAL ... portEXIT_CRITICAL) lets only one
// task inside at a time. Keep what's inside very short.
// =====================================================================

#include "input.h"

const int QUEUE_SIZE = 3;                  // enough for fast double turns

static Direction queue[QUEUE_SIZE];
static int queueCount = 0;
static bool restartRequested = false;
static bool pauseRequested = false;
static bool modeRequested = false;
static WallMode requestedMode = MODE_WALLS;

// The lock that guards everything above.
static portMUX_TYPE inputLock = portMUX_INITIALIZER_UNLOCKED;


// ---------- Used by input sources ----------

void inputPushDirection(Direction d) {
  portENTER_CRITICAL(&inputLock);
  // Pressing the same direction twice in a row adds nothing new.
  bool repeat = (queueCount > 0 && queue[queueCount - 1] == d);
  // If the queue is full the press is dropped: three pending turns is
  // already more than anyone can use in one tick.
  if (!repeat && queueCount < QUEUE_SIZE) {
    queue[queueCount] = d;
    queueCount++;
  }
  portEXIT_CRITICAL(&inputLock);
}


void inputRequestRestart() {
  portENTER_CRITICAL(&inputLock);
  restartRequested = true;
  portEXIT_CRITICAL(&inputLock);
}


void inputRequestPause() {
  portENTER_CRITICAL(&inputLock);
  pauseRequested = true;
  portEXIT_CRITICAL(&inputLock);
}


void inputRequestMode(WallMode m) {
  portENTER_CRITICAL(&inputLock);
  requestedMode = m;
  modeRequested = true;
  portEXIT_CRITICAL(&inputLock);
}


// ---------- Used by the main loop ----------

void inputUpdate() {
  // Serial keys still work, handy for testing without the phone.
  while (Serial.available() > 0) {
    char c = Serial.read();
    switch (tolower(c)) {
      case 'w': inputPushDirection(DIR_UP);    break;
      case 's': inputPushDirection(DIR_DOWN);  break;
      case 'a': inputPushDirection(DIR_LEFT);  break;
      case 'd': inputPushDirection(DIR_RIGHT); break;
      case 'r': inputRequestRestart();         break;
      case 'p': inputRequestPause();           break;
      default:  break;                         // ignore Enter and anything else
    }
  }
}


bool inputNextDirection(Direction &d) {
  bool found = false;
  portENTER_CRITICAL(&inputLock);
  if (queueCount > 0) {
    d = queue[0];
    for (int i = 1; i < queueCount; i++) {    // shift the rest forward
      queue[i - 1] = queue[i];
    }
    queueCount--;
    found = true;
  }
  portEXIT_CRITICAL(&inputLock);
  return found;
}


bool inputRestartRequested() {
  portENTER_CRITICAL(&inputLock);
  bool requested = restartRequested;
  restartRequested = false;                   // report it only once
  portEXIT_CRITICAL(&inputLock);
  return requested;
}


bool inputPauseRequested() {
  portENTER_CRITICAL(&inputLock);
  bool requested = pauseRequested;
  pauseRequested = false;
  portEXIT_CRITICAL(&inputLock);
  return requested;
}


bool inputModeRequested(WallMode &m) {
  portENTER_CRITICAL(&inputLock);
  bool requested = modeRequested;
  if (requested) {
    m = requestedMode;
    modeRequested = false;
  }
  portEXIT_CRITICAL(&inputLock);
  return requested;
}


void inputClear() {
  portENTER_CRITICAL(&inputLock);
  queueCount = 0;
  pauseRequested = false;
  portEXIT_CRITICAL(&inputLock);
}
