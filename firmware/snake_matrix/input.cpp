// =====================================================================
// input.cpp
// ---------------------------------------------------------------------
// Why a queue? The snake only moves once per tick (every 250 ms). If you
// press "up" then "left" quickly within one tick, a single "latest key"
// variable would lose the "up". A queue keeps both, and the game uses
// one per tick, in order. That's what makes quick turns feel reliable.
// =====================================================================

#include "input.h"

const int QUEUE_SIZE = 3;                  // enough for fast double turns

static Direction queue[QUEUE_SIZE];
static int queueCount = 0;
static bool restartRequested = false;


// Adds a direction to the end of the queue.
static void pushDirection(Direction d) {
  // Pressing the same key twice in a row adds nothing new.
  if (queueCount > 0 && queue[queueCount - 1] == d) {
    return;
  }
  // If the queue is full, the key is dropped. Three pending turns is
  // already more than anyone can use in one tick.
  if (queueCount < QUEUE_SIZE) {
    queue[queueCount] = d;
    queueCount++;
  }
}


void inputUpdate() {
  // Read every character that has arrived. Serial.available() says how
  // many are waiting, so this never pauses the program.
  while (Serial.available() > 0) {
    char c = Serial.read();
    switch (tolower(c)) {             // accept W or w
      case 'w': pushDirection(DIR_UP);    break;
      case 's': pushDirection(DIR_DOWN);  break;
      case 'a': pushDirection(DIR_LEFT);  break;
      case 'd': pushDirection(DIR_RIGHT); break;
      case 'r': restartRequested = true;  break;
      default:  break;                // ignore Enter and anything else
    }
  }
}


bool inputNextDirection(Direction &d) {
  // The & in "Direction &d" means we fill in the caller's variable
  // directly, so this function can hand back a direction AND say
  // (true/false) whether there was one.
  if (queueCount == 0) {
    return false;
  }
  d = queue[0];
  // Shift the rest forward by one.
  for (int i = 1; i < queueCount; i++) {
    queue[i - 1] = queue[i];
  }
  queueCount--;
  return true;
}


bool inputRestartRequested() {
  if (restartRequested) {
    restartRequested = false;         // report it only once
    return true;
  }
  return false;
}


void inputClear() {
  queueCount = 0;
}
