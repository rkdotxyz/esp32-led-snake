// =====================================================================
// input.h
// ---------------------------------------------------------------------
// Turns keypresses into game commands. For now the keys come from the
// USB serial connection; in phase 6 the phone will feed the same queue.
//
//   W A S D = up, left, down, right      R = restart
// =====================================================================

#pragma once

#include "snake_game.h"   // for the Direction type

void inputUpdate();                         // read any new keys; call every loop()
bool inputNextDirection(Direction &d);      // take the oldest queued direction, if any
bool inputRestartRequested();               // true once after R is pressed
void inputClear();                          // forget queued directions (on a new game)
