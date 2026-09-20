// =====================================================================
// config.h
// ---------------------------------------------------------------------
// Every setting you might want to tweak, in one place. Other files
// #include this, so changing a number here changes it everywhere.
// =====================================================================

// #pragma once stops this file being pasted in twice if two other files
// both #include it. Every .h file in this project starts with it.
#pragma once

#include <FastLED.h>   // for the CRGB colour type used below


// ---------- Hardware ----------
const int DATA_PIN = 2;                              // GPIO2 (D2)
const int MATRIX_WIDTH = 32;                         // columns
const int MATRIX_HEIGHT = 8;                         // rows
const int NUM_LEDS = MATRIX_WIDTH * MATRIX_HEIGHT;   // 256


// ---------- Power ----------
const uint8_t BRIGHTNESS = 40;         // 0-255
const uint32_t MAX_MILLIAMPS = 300;    // FastLED power cap (phase 2)


// ---------- Game ----------
const unsigned long TICK_MS = 250;     // time between snake steps. Lower = faster
const int START_LENGTH = 3;            // segments at the start of a game


// ---------- Colours ----------
const CRGB HEAD_COLOUR = CRGB(120, 255, 120);   // pale green, brighter than the body
const CRGB BODY_COLOUR = CRGB(0, 160, 40);      // green
const CRGB FOOD_COLOUR = CRGB(255, 30, 0);      // red-orange
const CRGB DEAD_COLOUR = CRGB(255, 0, 0);       // head turns red on game over
