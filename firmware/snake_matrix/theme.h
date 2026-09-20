// =====================================================================
// theme.h
// ---------------------------------------------------------------------
// How the snake and food look: their colours (chosen on the phone) and
// the effects on top: a pulsing head, a body fading toward the tail,
// gently pulsing food, and a sparkle when food is eaten.
//
// Colours are passed around as 0xRRGGBB numbers (uint32_t), the same
// format as web colours like #00ff40, so they travel easily between the
// phone, the flash storage and FastLED.
// =====================================================================

#pragma once

#include "config.h"

// ---------- Colours ----------
void themeSetSnakeColour(uint32_t rgb);    // stored as given; call themeNormalise first
void themeSetFoodColour(uint32_t rgb);
uint32_t themeSnakeColour();
uint32_t themeFoodColour();
uint32_t themeNormalise(uint32_t rgb);     // same hue, full brightness (see theme.cpp)

// ---------- What to draw ----------
CRGB themeHead();                          // pulsing head colour, right now
CRGB themeBody(int i, int length);         // body segment i (1 = neck), fading to the tail
CRGB themeFood();                          // gently pulsing food colour, right now

// ---------- Eat sparkle ----------
void themeStartSparkle(int x, int y);      // start a sparkle where food was eaten
void themeDrawSparkle();                   // draw it, if one is still running
