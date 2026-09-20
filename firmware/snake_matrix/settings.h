// =====================================================================
// settings.h
// ---------------------------------------------------------------------
// Settings that survive power-off. The ESP32 keeps them in a small area
// of its flash memory called NVS ("non-volatile storage"), as named
// key/value pairs, a bit like localStorage in a browser.
//
// Three settings: the wall mode, the snake colour and the food colour.
// =====================================================================

#pragma once

#include "snake_game.h"   // for WallMode
#include "config.h"       // for the default colours

void settingsBegin();                     // open the storage; call once in setup()
WallMode settingsWallMode();              // the saved mode (walls if never saved)
void settingsSaveWallMode(WallMode m);    // remember a new mode

uint32_t settingsSnakeColour();           // saved colour as 0xRRGGBB (default if never saved)
uint32_t settingsFoodColour();
void settingsSaveSnakeColour(uint32_t rgb);
void settingsSaveFoodColour(uint32_t rgb);
