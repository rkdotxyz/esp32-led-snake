// =====================================================================
// settings.h
// ---------------------------------------------------------------------
// Settings that survive power-off. The ESP32 keeps them in a small area
// of its flash memory called NVS ("non-volatile storage"), as named
// key/value pairs, a bit like localStorage in a browser.
//
// For now there's one setting: the wall mode. Themes join it in phase 8.
// =====================================================================

#pragma once

#include "snake_game.h"   // for WallMode

void settingsBegin();                     // open the storage; call once in setup()
WallMode settingsWallMode();              // the saved mode (walls if never saved)
void settingsSaveWallMode(WallMode m);    // remember a new mode
