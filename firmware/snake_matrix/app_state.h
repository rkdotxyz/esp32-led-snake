// =====================================================================
// app_state.h
// ---------------------------------------------------------------------
// The screens the whole device can be on. Only one is active at a time,
// and the main sketch moves between them (a "state machine"):
//
//   ATTRACT --direction--> PLAYING <--pause/direction--> PAUSED
//      ^                      |
//      |                   crashes
//      |                      v
//      +---- after 3 s --- SCORE <--animation done-- GAME_OVER
//
// Restart jumps back to ATTRACT from anywhere.
//
// Why a separate file? Functions in the .ino take an AppState, and the
// Arduino IDE's auto-generated declarations only understand types that
// come from an #include (the OLED project's "does not name a type" bug).
// =====================================================================

#pragma once

enum AppState {
  STATE_ATTRACT,     // demo animation, waiting for the first direction
  STATE_PLAYING,     // the game is running
  STATE_PAUSED,      // game frozen on screen
  STATE_GAME_OVER,   // crash animation
  STATE_SCORE        // final score on the matrix
};
