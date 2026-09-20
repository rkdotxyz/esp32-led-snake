# Build log

## Phase 1: Tools and repo

**Date:** 2026-09-20

**Versions installed**
- Arduino IDE: 2.3.10 (CLI 1.5.1)
- esp32 board package: 3.3.11 (esptool 5.3.1)
- FastLED: 3.10.5
- Async TCP (ESP32Async): 3.5.0
- ESP Async WebServer (ESP32Async): 3.12.1

**Board**
- Chip: ESP32-D0WD-V3, revision v3.1, dual core, 240 MHz, 40 MHz crystal
- Port on macOS: `/dev/cu.usbserial-0001`
- Arduino IDE board setting: DOIT ESP32 DEVKIT V1

**What I did**
- Updated Arduino IDE, installed the esp32 board package and the three libraries.
- Moved libraries left over from the first Snake build out of the libraries folder so they couldn't clash.
- Set up the repo skeleton: README, license, parts list, gotchas, build log, test sketches.
- Flashed `t01_blink`: 272,660 bytes (20% of program storage).
- Flashed `t01_libraries`: 390,015 bytes (29% of program storage). All three libraries compile together with board package 3.3.11.

**What broke and how I fixed it**
- Library Manager showed no results while the board package was still installing. Waited for the install to finish, then searched again.
- The IDE was still set to Arduino UNO from a previous project. Switched the board to DOIT ESP32 DEVKIT V1 and the port to the ESP32's.

**Result**
- [x] t01_blink uploads and runs
- [x] t01_libraries compiles and uploads
- [x] Repo pushed to GitHub, tagged v0.1


## Phase 2: Power and first light

**Date:** 2026-09-20

**Parts used**
- 330 Ω resistor (orange, orange, brown, gold) in series on the data line
- 100 µF 25 V electrolytic capacitor across 5 V and GND (1000 µF was planned; 100 µF is enough on USB power)
- Breadboard and jumper wires; the ESP32 sits off the breadboard with jumpers straight to its pins

**Wiring (USB only)**
- ESP32 VIN → matrix 5V (red)
- ESP32 GND → matrix GND (white)
- ESP32 D2 → 330 Ω → matrix DIN (green)
- Capacitor across + and − rails, stripe on −

**What I did**
- Wrote and ran `t02_first_pixels`: colour test on LED 0, then a single dot chasing through all 256 LEDs.
- Wrote and ran `t04_power_cap`: whole panel white, stepping requested brightness from 5 to 255.

**Results**
- Colour order: LED 0 showed red, green, blue in order, so `GRB` is correct.
- Wiring pattern: LED 0 is top left. The chain runs column by column: down column 0, up column 1, down column 2, and so on (8 LEDs per column).
- Power cap: with every LED white, the 300 mA cap limits brightness to 6/255 (FastLED estimates ~299 mA). Even at 5/255 the estimate is ~257 mA, because FastLED counts a baseline for every powered LED plus the ESP32.
- No brownout resets during the power test.

**What broke and how I fixed it**
- Serial Monitor showed the chase running, but no LEDs lit. The breadboard's power rails are split in the middle (around column 30): VIN/GND went into one half and the matrix power into the other, so the matrix had no power. Fixed by moving the matrix's red and white wires into the same rail section as VIN and GND.
- Garbled characters in Serial Monitor right after upload: the ESP32's boot messages run at 74880 baud. Normal; ignore.
- A burst of repeated lines with identical timestamps: Serial Monitor delivering lines buffered while it reconnected. Harmless.

**Checklist**
- [x] t02_first_pixels: colour test and chase work
- [x] t04_power_cap: cap holds, no brownout
- [x] Resistor and capacitor back in circuit, t02 re-tested
- [ ] Wiring photo saved in hardware/photos/
- [x] Committed and tagged v0.2


## Phase 3: Map the grid

**Date:** 2026-09-20

**What I did**
- Wrote `t03_xy_mapping` with an `XY(x, y)` function that turns a grid position into the LED's number on the strip, and a `setPixel(x, y, colour)` helper that ignores positions off the grid.
- Added typed commands in Serial Monitor: `x y` lights a single pixel and prints its LED number; `demo` restarts the test patterns.
- No wiring changes.

**The mapping**
- x = column, 0 (left) to 31 (right); y = row, 0 (top) to 7 (bottom). Matrix held with the plug on the left.
- Column x starts at LED `x * 8`.
- Even columns run top to bottom: `x * 8 + y`.
- Odd columns run bottom to top: `x * 8 + (7 - y)`.
- Off-grid positions return -1, so nothing is drawn.

**Results**
- Corners: red top left, green top right, blue bottom left, yellow bottom right.
- Border, diagonals, column sweep and row sweep all drew correctly.
- Hand-typed positions matched predictions: `0 0` → 0, `1 0` → 15, `5 3` → 44, `31 0` → 255, `31 7` → 248, `32 0` → off the grid.

**What broke and how I fixed it**
- Nothing this phase.

**Checklist**
- [x] All five test patterns correct
- [x] Hand-typed positions match predictions
- [x] Committed and tagged v0.3


## Phase 4: Snake core, played over Serial

**Date:** 2026-09-20

**What I did**
- Started the main sketch in `firmware/snake_matrix/`, split into modules:
  - `config.h`: pin, matrix size, power cap, game speed, colours
  - `display.h/.cpp`: FastLED setup, `XY()` mapping and pixel drawing (the only code that touches LEDs)
  - `snake_game.h/.cpp`: the rules only (move, eat, grow, walls, self-collision, food placement); no LED or Serial code
  - `input.h/.cpp`: W A S D and R from Serial, fed through a small direction queue so quick double turns aren't lost
  - `snake_matrix.ino`: ties it together; reads input every loop, steps the game every `TICK_MS` (250 ms), redraws
- Custom types (`Direction`, `Point`) live in `.h` files, which avoids Arduino IDE's auto-prototype problem.

**Results**
- Playable from Serial Monitor: type a letter, then Enter (several letters at once queue several turns).
- Snake moves, turns, eats, grows by one per food, and dies on walls or on itself; R restarts.

**What broke and how I fixed it**
- Real-time play from a terminal didn't work. `screen` exited immediately ("screen is terminating"). `tio` connected but didn't respond to keys, most likely because opening the port toggles the ESP32's DTR/RTS reset lines. Parked rather than debugged: the browser controller replaces keyboard play anyway.
- VS Code shows false errors on the sketch because it doesn't know where the Arduino libraries are. Arduino IDE compiles it cleanly, which is what counts.

**Decision**
- Swapped phases: the browser/phone controller comes next (phase 5), the attract screen and game-over animation after it (phase 6).

**Checklist**
- [x] Moves and turns with W A S D; reversing is ignored
- [x] Eating grows the snake and prints the score
- [x] Wall and self collisions end the game; R restarts
- [x] Committed and tagged v0.4