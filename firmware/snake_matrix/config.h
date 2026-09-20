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


// ---------- WiFi hotspot (new in phase 5) ----------
// The ESP32 creates its own WiFi network. Join it from your phone or
// laptop, then open http://snake.local (or http://192.168.4.1).
// `const char* const` = a piece of text that can never be changed.
const char* const AP_SSID = "ESP32-Snake";     // the network name you'll see
const char* const AP_PASSWORD = "playsnake";   // must be at least 8 characters
const char* const HOSTNAME = "snake";          // gives the address snake.local
