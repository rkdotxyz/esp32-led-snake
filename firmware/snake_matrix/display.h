// =====================================================================
// display.h
// ---------------------------------------------------------------------
// The only part of the program that talks to the LEDs. Everything else
// draws with simple (x, y) calls and never thinks about LED numbers,
// zigzag wiring or FastLED.
//
// A .h file is the "menu": it lists what this module offers. The .cpp
// file with the same name is the "kitchen", where the work happens.
// =====================================================================

#pragma once

#include "config.h"

void displayBegin();                                // set up FastLED; call once in setup()
void displayClear();                                // turn every pixel off (in the buffer)
void displaySetPixel(int x, int y, CRGB colour);    // colour one pixel; off-grid is ignored
void displayShow();                                 // send the buffer to the matrix

int XY(int x, int y);                               // grid position -> LED number, or -1
