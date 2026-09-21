# Phase 8: Colours and effects (v2.0)

**Goal:** snake and food colours chosen in Settings (presets or any colour), and effects on the matrix: a pulsing head, a body fading toward the tail, glowing food and a sparkle when you eat. No wiring changes.

**Code at the end of this phase:** [`v2.0`](https://github.com/rkdotxyz/esp32-led-snake/tree/v2.0)

## Two clocks

Until now the matrix only redrew when the snake moved, four times a second: far too choppy for a pulse or a sparkle. Now two timers run independently:

| Clock | Every | What happens |
|---|---|---|
| Movement (`TICK_MS`) | 250 ms | The snake moves one square |
| Drawing (`FRAME_MS`) | 30 ms | The picture is redrawn |

```cpp
if (now - lastTick >= TICK_MS) {
  lastTick = now;
  // ... move the snake ...
}
drawIfDue(now);   // redraws every FRAME_MS, whether or not the snake moved
```

Separating "when things move" from "when things are drawn" is how nearly every game works.

## The theme module

`theme.h` / `.cpp` holds the two colours and draws everything that depends on them.

**Colours as one number.** A colour travels as `0xRRGGBB`: the phone sends `#55aaff`, the ESP32 reads it as a number, saves that number to flash, and gives it straight to FastLED.

**Full-strength colours.** A picker allows dark colours like navy, which would vanish on the matrix at this brightness. `themeNormalise()` keeps the hue but raises the strongest channel to 255:

```cpp
uint32_t top = r;
if (g > top) top = g;
if (b > top) top = b;
if (top == 0) return 0xFFFFFF;     // black can't be shown on LEDs: use white
r = r * 255 / top;                 // #336699 -> #55aaff
g = g * 255 / top;
b = b * 255 / top;
```

The ESP32 sends the normalised colour back, so the phone shows the colour the matrix actually uses.

**Effects from the clock.** `beatsin8()` gives a smooth wave from the time; `blend()` mixes two colours:

```cpp
CRGB themeHead() {
  uint8_t whiteness = beatsin8(HEAD_PULSE_BPM, 40, 150);   // swings 40..150
  return blend(CRGB(snakeRgb), CRGB::White, whiteness);
}
```

The tail fade is arithmetic on the segment number, and the sparkle is a small cross of food-coloured pixels spreading one, then two pixels out while fading.

Drawing order matters: food, then sparkle, then body, then head, so the snake is always on top.

## Settings

The panel gains **Snake colour** and **Food colour** rows: preset circles plus a rainbow circle that opens the phone's colour picker. Details worth copying into your own projects:

- The picker sends on `change`, not `input`. `input` fires continuously while dragging, and each one would be a flash write.
- The page blocks scrolling (`touch-action: none`) for swipes, so the Settings card needs `touch-action: pan-y` to scroll.
- The attract screen uses the theme, so colour changes preview on the matrix straight away.
- Colours follow the same rule as edges: only between games.

## Tuning

In `config.h`: `HEAD_PULSE_BPM`, `TAIL_MIN_LEVEL` (tail tip brightness), `SPARKLE_MS`, and the default colours. Presets are in `PRESETS` in `index.html`; keep them full-strength (at least one channel `ff`) so they match what the ESP32 echoes back.

## What to check

- [ ] Presets and custom colours work for snake and food, and preview on the attract screen.
- [ ] A dark custom colour shows up bright.
- [ ] The head pulses, the body fades, the food glows, eating sparkles.
- [ ] Colours are remembered after unplugging.

## If something's wrong

| Symptom | Likely cause and fix |
|---|---|
| "HEAD_COLOUR was not declared" | An old file still uses the fixed colours. |
| No colour rows | Embed script not re-run, or cached page. |
| A preset rings the custom circle | The preset isn't full-strength. |
| Tail tip disappears | `TAIL_MIN_LEVEL` too low for your brightness. |

**Next:** [Phase 9: Sound and speed-up](09-sound-and-speed-up.md)
