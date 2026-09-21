# Test sketches

Small, single-purpose sketches used to bring the hardware up one step at a time. Each one checks one thing, so when something stops working later, they're the quickest way to find out whether the problem is the wiring, the power, the libraries or the game code.

Open any of them in Arduino IDE, upload, and open Serial Monitor at **115200 baud**.

| Sketch | Checks | Wiring needed | You should see |
|---|---|---|---|
| `t01_blink` | USB cable, port, board settings, Serial Monitor | None | The ESP32's blue LED blinks; Serial Monitor prints the chip model and board package version |
| `t01_libraries` | FastLED, Async TCP and ESP Async WebServer compile together | None | Uploads cleanly; Serial Monitor prints the FastLED version |
| `t02_first_pixels` | Data line, power, colour order, how the LEDs are chained | Matrix | LED 0 shows red, green, blue in turn, then one white dot walks through all 256 LEDs |
| `t03_xy_mapping` | The (x, y) → LED mapping | Matrix | Coloured corners, a border, diagonals and sweeping lines; type `x y` in Serial Monitor (line ending: New Line) to light one pixel |
| `t04_power_cap` | The firmware power cap holds on USB power | Matrix | Whole panel white but capped at a low brightness; no `BROWNOUT` resets |

## When to use them

- **Board not detected, or uploads fail:** `t01_blink`. If this fails, it's the cable, the port or the board setting, not your code.
- **Compile errors inside a library:** `t01_libraries`. Usually two copies of a library from different authors.
- **Matrix dark or glitchy:** `t02_first_pixels`. If the dot doesn't walk cleanly, check the power wires (both in the same breadboard rail section), the data wire (at the **DIN** end) and the resistor.
- **Drawing lands in the wrong place:** `t03_xy_mapping`. The corners show at a glance whether the matrix is flipped or rotated.
- **Random resets:** `t04_power_cap`. A `Reset reason: BROWNOUT` means too much current for USB; lower `MAX_MILLIAMPS`.

The main game lives in [`../snake_matrix`](../snake_matrix). The step-by-step build guide is in [`../../docs/guide`](../../docs/guide).
