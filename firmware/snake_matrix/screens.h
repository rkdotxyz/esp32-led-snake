// =====================================================================
// screens.h
// ---------------------------------------------------------------------
// Everything drawn outside actual gameplay: the attract animation, the
// game-over animation and the final score.
//
// Each function takes `elapsed`: milliseconds since that screen started.
// A frame depends only on the time, so the main sketch can call these as
// often as it likes, and the animation always runs at the same speed.
// =====================================================================

#pragma once

void screensDrawAttract(unsigned long elapsed);
bool screensDrawGameOver(unsigned long elapsed);          // true once the animation has finished
void screensDrawScore(int score, unsigned long elapsed);
