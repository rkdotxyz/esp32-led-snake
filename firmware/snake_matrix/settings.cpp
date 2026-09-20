// =====================================================================
// settings.cpp
// ---------------------------------------------------------------------
// Uses the Preferences library, built into the ESP32 board package.
//
// Flash memory wears out after many thousands of writes to the same
// spot, so we only save when something actually changes, never in a
// loop. A mode switch now and then is nothing.
// =====================================================================

#include <Preferences.h>
#include "settings.h"

static Preferences prefs;

// Keys and the "namespace" (a folder for our keys) are short text names.
// Namespace and key names can be at most 15 characters.
static const char* NAMESPACE = "snake";
static const char* KEY_WRAP = "wrap";
static const char* KEY_SNAKE = "snake";
static const char* KEY_FOOD = "food";


void settingsBegin() {
  // false = open for reading AND writing.
  prefs.begin(NAMESPACE, false);
}


WallMode settingsWallMode() {
  // Stored as true/false: true means wrap. The second argument is the
  // default, used the very first time, before anything has been saved.
  bool wrap = prefs.getBool(KEY_WRAP, false);
  return wrap ? MODE_WRAP : MODE_WALLS;
}


void settingsSaveWallMode(WallMode m) {
  prefs.putBool(KEY_WRAP, m == MODE_WRAP);
}


// Colours are stored as whole numbers (0xRRGGBB). getUInt / putUInt read
// and write an unsigned 32-bit number; the second argument of getUInt is
// the default for a board that has never saved one.

uint32_t settingsSnakeColour() {
  return prefs.getUInt(KEY_SNAKE, DEFAULT_SNAKE_RGB);
}


uint32_t settingsFoodColour() {
  return prefs.getUInt(KEY_FOOD, DEFAULT_FOOD_RGB);
}


void settingsSaveSnakeColour(uint32_t rgb) {
  prefs.putUInt(KEY_SNAKE, rgb);
}


void settingsSaveFoodColour(uint32_t rgb) {
  prefs.putUInt(KEY_FOOD, rgb);
}
