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
const uint32_t MAX_MILLIAMPS = 300;    // FastLED power cap; keeps USB power safe


// ---------- Game ----------
// The snake speeds up as it eats: each food takes
// SPEEDUP_PER_FOOD_MS off the time between steps, until MIN_TICK_MS.
// With these numbers it reaches top speed after 26 foods.
const unsigned long START_TICK_MS = 250;      // time between steps at the start
const unsigned long MIN_TICK_MS = 120;        // fastest it ever gets
const unsigned long SPEEDUP_PER_FOOD_MS = 5;  // how much faster per food
const int START_LENGTH = 3;            // segments at the start of a game


// ---------- Screens and animation ----------
const unsigned long FRAME_MS = 30;          // animations redraw about 33 times a second
const unsigned long ATTRACT_STEP_MS = 90;   // attract-screen snake speed
const unsigned long SCORE_SHOW_MS = 3000;   // how long the final score stays up


// ---------- Colours ----------
// Snake and food colours are chosen in the controller's Settings and
// saved in flash. These are only the defaults for a fresh board.
// Written as 0xRRGGBB, the same as a web colour like #00ff40.
const uint32_t DEFAULT_SNAKE_RGB = 0x00FF40;    // green
const uint32_t DEFAULT_FOOD_RGB = 0xFF2000;     // red-orange

const CRGB DEAD_COLOUR = CRGB(255, 0, 0);       // the crashed snake
const CRGB SCORE_COLOUR = CRGB(255, 170, 0);    // amber digits for the final score


// ---------- Effects ----------
const uint8_t HEAD_PULSE_BPM = 50;        // head pulses toward white, ~50 times a minute
const uint8_t TAIL_MIN_LEVEL = 60;        // tail tip brightness, out of 255 (the neck is 255)
const unsigned long SPARKLE_MS = 400;     // how long the eat sparkle lasts


// ---------- WiFi hotspot ----------
// The ESP32 creates its own WiFi network. Join it from your phone or
// laptop, then open http://snake.local (or http://192.168.4.1).
// `const char* const` = a piece of text that can never be changed.
const char* const AP_SSID = "ESP32-Snake";     // the network name you'll see
const char* const AP_PASSWORD = "playsnake";   // must be at least 8 characters
const char* const HOSTNAME = "snake";          // gives the address snake.local
