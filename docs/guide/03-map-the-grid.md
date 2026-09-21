# Phase 3: Map the grid

**Goal:** let the code say "column 5, row 3" instead of "LED 44". No wiring changes.

**Code at the end of this phase:** [`v0.3`](https://github.com/rkdotxyz/esp32-led-snake/tree/v0.3) (`firmware/tests/t03_xy_mapping`)

## The idea

The matrix is one long strip of 256 LEDs folded into columns. Phase 2 showed how it's folded: LED 0 is top left, even columns run down, odd columns run up.

```
        x=0   x=1   x=2   x=3
y=0      0     15    16    31
y=1      1     14    17    30
y=2      2     13    18    29
y=3      3     12    19    28
y=4      4     11    20    27
y=5      5     10    21    26
y=6      6      9    22    25
y=7      7      8    23    24
```

`x` is the column (0–31, left to right), `y` is the row (0–7, top to bottom). Hold the matrix with the plug on the left.

Each column holds 8 LEDs, so column `x` starts at LED `x × 8`. Even columns add `y`; odd columns count from the bottom, so they add `7 − y`.

```cpp
int XY(int x, int y) {
  if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
    return -1;                                   // off the grid
  }
  int columnStart = x * HEIGHT;
  if (x % 2 == 0) {
    return columnStart + y;                      // even columns run down
  } else {
    return columnStart + (HEIGHT - 1 - y);       // odd columns run up
  }
}
```

Returning `-1` off the grid, with a `setPixel()` helper that ignores it, means drawing code never has to check bounds. The game relies on that later.

## t03_xy_mapping

Cycles through five patterns, each catching a different mistake:

| Pattern | Correct looks like | Proves |
|---|---|---|
| Corners | Red top left, green top right, blue bottom left, yellow bottom right | Nothing flipped or rotated |
| Border | An unbroken rectangle | The edges are right |
| Diagonals | Four clean slopes down to the right | Odd and even columns both map correctly |
| Column sweep | A straight vertical line moving right | Every column whole and in order |
| Row sweep | A straight horizontal line moving down | Every row whole and in order |

It also reads commands typed into Serial Monitor. Set the line-ending dropdown to **New Line**, then type two numbers, like `5 3`, and press Enter to light that pixel. Type `demo` to restart the patterns. Characters are collected one at a time and handled when Enter arrives, so `loop()` never waits:

```cpp
while (Serial.available() > 0) {
  char c = Serial.read();
  if (c == '\n' || c == '\r') {
    if (inputLine.length() > 0) handleCommand(inputLine);
    inputLine = "";
  } else {
    inputLine += c;
  }
}
```

## Try it by hand

Work out each LED number before pressing Enter:

| Type | Lights | LED |
|---|---|---|
| `0 0` | top left | 0 |
| `1 0` | top of the second column | 15 |
| `5 3` | sixth column, fourth row | 44 |
| `31 0` | top right | 255 |
| `31 7` | bottom right | 248 |
| `32 0` | nothing: off the grid | Serial Monitor says so |

## What to check

- [ ] All five patterns look as described.
- [ ] Your predictions match.

## If something's wrong

| Symptom | Likely cause and fix |
|---|---|
| Red corner bottom left | The matrix is upside down compared with phase 2. |
| Red corner at the right | The plug is on the right. Turn the matrix around. |
| Jagged diagonals, broken border | Odd and even directions swapped: swap the two `return` lines in `XY()`. |
| Typing does nothing | Line ending is "No line ending". Set it to New Line. |

**Next:** [Phase 4: Snake core](04-snake-core.md)
