# Phase 7: Walls and wrap (v1.1)

**Goal:** two rules for the edges, chosen in a Settings panel between games, and remembered after power-off. No wiring changes.

**Code at the end of this phase:** [`v1.1`](https://github.com/rkdotxyz/esp32-led-snake/tree/v1.1)

## Wrapping with `%`

In wrap mode, the snake leaves one edge and comes back in at the opposite one. The remainder operator does the folding: `32 % 32 = 0`, so going off the right edge lands on the left.

The left edge has a catch: in C++, `-1 % 32` is `-1`, not `31`. So the width is added first:

```cpp
if (offBoard) {
  if (wallMode == MODE_WALLS) {
    over = true;
    reason = "hit a wall";
    return;
  }
  next.x = (int8_t)((next.x + MATRIX_WIDTH) % MATRIX_WIDTH);
  next.y = (int8_t)((next.y + MATRIX_HEIGHT) % MATRIX_HEIGHT);
}
```

## Remembering it: Preferences

The ESP32 keeps a small key/value store in flash, much like a browser's localStorage. The new `settings.cpp` saves one true/false value:

```cpp
prefs.begin("snake", false);               // open for reading and writing
bool wrap = prefs.getBool("wrap", false);  // false the very first time
prefs.putBool("wrap", true);               // survives power-off
```

Flash wears out after many thousands of writes, so settings are only saved when they actually change. Names are limited to 15 characters.

## The Settings panel

A Settings button next to Pause and Restart opens a panel over the controller with **Edges: Walls | Wrap**.

- **Only between games.** The button is greyed out while a game is playing or paused. The ESP32 enforces it too, since a second phone or a cached page could still send a request.
- **The ESP32 is the source of truth.** Tapping Wrap sends a request; the page only highlights Wrap when the ESP32 confirms. Two phones always agree.

New messages: `mode walls` / `mode wrap`, in both directions.

## What to check

- [ ] Settings opens a panel; Wrap highlights when tapped; Done or tapping outside closes it.
- [ ] Wrap works on all four edges; running into yourself still ends the game.
- [ ] Settings is greyed out during a game.
- [ ] Unplug and replug: the mode is remembered.

## Design change after testing

The first version put the Walls/Wrap switch on the main controller and allowed changes mid-game. Changing the rules mid-game felt wrong, so edges moved into a Settings panel, available only between games. That panel became the home for colours in phase 8.

## If something's wrong

| Symptom | Likely cause and fix |
|---|---|
| "WallMode does not name a type" | `snake_game.h` not updated. |
| "Preferences.h: No such file" | Board isn't set to the ESP32. |
| Tapping Wrap highlights nothing | `web_control.cpp` is missing the two `mode` messages. |
| Snake jumps to the wrong row when wrapping | `% MATRIX_WIDTH` and `% MATRIX_HEIGHT` swapped. |
| Mode forgotten after unplugging | `settingsBegin()` must run in `setup()` before reading the mode. |

**Next:** [Phase 8: Colours and effects](08-colours-and-effects.md)
