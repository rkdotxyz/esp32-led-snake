// =====================================================================
// input.h
// ---------------------------------------------------------------------
// The single place every control ends up, whatever it came from.
// Serial keys and the web controller both feed the same queue, so the
// game doesn't know or care where a command came from.
//
// Serial keys: W A S D = up, left, down, right   R = restart   P = pause
// =====================================================================

#pragma once

#include "snake_game.h"   // for the Direction type


// ---------- Used by the main loop ----------
void inputUpdate();                         // read Serial keys; call every loop()
bool inputNextDirection(Direction &d);      // take the oldest queued direction, if any
bool inputRestartRequested();               // true once after a restart request
bool inputPauseRequested();                 // true once after a pause request
void inputClear();                          // forget everything pending (new game)


// ---------- Used by input sources (Serial, the web controller) ----------
void inputPushDirection(Direction d);       // queue a direction
void inputRequestRestart();                 // ask for a new game
void inputRequestPause();                   // ask to pause or resume
