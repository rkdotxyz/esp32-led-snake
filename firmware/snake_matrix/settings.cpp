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
