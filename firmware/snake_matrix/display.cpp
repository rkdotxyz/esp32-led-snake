// =====================================================================
// display.cpp
// ---------------------------------------------------------------------
// How the display functions promised in display.h actually work.
// XY() is the mapping worked out with firmware/tests/t03_xy_mapping.
// =====================================================================

#include "display.h"

// The LED buffer. `static` means only this file can see it: no other
// part of the program can touch the LEDs directly, only through the
// functions below. That keeps all LED handling in one place.
static CRGB leds[NUM_LEDS];


void displayBegin() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_MILLIAMPS);
  FastLED.setBrightness(BRIGHTNESS);

  // Dithering makes very dim colours flicker between two levels to fake
  // in-between brightness. On the snake's faded tail that shows as
  // shimmer, so it's switched off.
  FastLED.setDither(DISABLE_DITHER);

  FastLED.clear(true);
}


void displayClear() {
  FastLED.clear();   // buffer only; nothing changes until displayShow()
}


void displaySetPixel(int x, int y, CRGB colour) {
  int i = XY(x, y);
  if (i >= 0) {
    leds[i] = colour;
  }
}


void displayShow() {
  FastLED.show();
}


// Column-by-column zigzag, LED 0 top left, as found with
// firmware/tests/t02_first_pixels.
int XY(int x, int y) {
  if (x < 0 || x >= MATRIX_WIDTH || y < 0 || y >= MATRIX_HEIGHT) {
    return -1;                                         // off the grid
  }
  int columnStart = x * MATRIX_HEIGHT;
  if (x % 2 == 0) {
    return columnStart + y;                            // even columns run down
  } else {
    return columnStart + (MATRIX_HEIGHT - 1 - y);      // odd columns run up
  }
}
