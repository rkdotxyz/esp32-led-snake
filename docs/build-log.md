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


## Phase 5: Browser controller

**Date:** 2026-09-20

**What I did**
- The ESP32 now runs its own WiFi hotspot (`ESP32-Snake`) and serves a controller page at `http://snake.local` (fallback `http://192.168.4.1`).
- `controller/index.html`: d-pad, swipe anywhere (chained swipes in one drag), arrow keys / W A S D on a laptop, Pause and Restart, live score and connection status. Reconnects automatically.
- `tools/embed_page.py` copies the page into `firmware/snake_matrix/controller_page.h` as a C++ raw string. Run it after every change to the page.
- `web_control.h/.cpp`: hotspot, mDNS name, web server, and a WebSocket at `/ws` for instant two-way messages (`up`, `restart`, `pause` in; `score`, `state`, `reason` out).
- `input.cpp`: any source can push commands; the queue is guarded by a lock because the web server runs in a separate task from `loop()`.
- Game flow: a new game waits for the first direction; the last controller disconnecting mid-game auto-pauses it.
- `display` and `snake_game` unchanged.

**Results**
- Playable from iPhone and laptop browsers; score and state update live.

**What broke and how I fixed it**
- The embed script printed nothing: `embed_page.py` was empty (created but code not pasted/saved). Pasted, saved, re-ran.

**Ideas for later (phase 9)**
- Sound on the phone for eating, hitting a wall and dying (Web Audio).
- Haptics on death and wall hits. Safari doesn't support `navigator.vibrate`, so iPhone needs a workaround; to be investigated.

**Checklist**
- [x] Hotspot, page and WebSocket connect
- [x] D-pad, swipes and keyboard all steer
- [x] Pause, auto-pause on disconnect, restart
- [x] Committed and tagged v0.5


## Phase 6: Attract screen, game over, score (v1.0)

**Date:** 2026-09-20

**What I did**
- Replaced the paused/started/over flags with an explicit state machine (`app_state.h`): Attract → Playing ⇄ Paused → GameOver → Score → Attract. Every change goes through `enterState()`, which logs it and updates the phones. Restart returns to Attract from anywhere.
- `screens.h/.cpp`:
  - **Attract:** a demo snake follows a route that visits all 256 squares once and loops (zigzag across columns 1–31, back up column 0), eating apples and growing.
  - **Game over:** the snake flashes red three times, then crumbles from the tail.
  - **Score:** a custom 5×6 digit font on rows 1–6, centred, counting up with leading zeros to the final score's length.
- Animations are drawn purely from elapsed time since the screen started, so they never drift.
- `display`: added `displayFill()`; disabled FastLED dithering so dim colours don't shimmer.
- Controller page: game-over message now says any direction plays again.

**Design changes after testing**
- Attract screen: removed the dim background glow; just the snake and apples on a dark board looks cleaner.
- Score: moved from a 5×7 font (top-aligned, bottom row unused) to 5×6 with the top and bottom rows empty.
- Score count-up: zero-padded (00, 01 … 12) so multi-digit scores don't look right-aligned while counting.

**Results**
- Full loop works: attract → play → crash → score → attract, controlled from the phone.

**Checklist**
- [x] Attract, game over and score screens look right
- [x] Pause, auto-pause and restart still work
- [x] Demo clip recorded in docs/media/
- [x] Committed and tagged v1.0


## Phase 7: Walls and wrap (v1.1)

**Date:** 2026-09-20

**What I did**
- `snake_game`: new `WallMode` (walls / wrap). In wrap mode the head re-enters at the opposite edge using `(x + WIDTH) % WIDTH`; the `+ WIDTH` is needed because in C++ `-1 % 32` is `-1`, not `31`.
- `settings.h/.cpp`: saves the mode in the ESP32's flash with the Preferences library (namespace `snake`, key `wrap`). Only written when the mode actually changes, to spare the flash.
- `input` / `web_control`: new `mode walls` / `mode wrap` messages, passed through the same locked queue as other commands.
- Controller page: a Settings button opens a panel with the Edges choice. The page highlights a mode only after the ESP32 confirms it.

**Design changes after testing**
- Edges moved from the main controller into a Settings panel, and can only change between games. The Settings button is disabled during a game, and the ESP32 also rejects mid-game requests. The panel will hold colour settings in phase 8.

**Results**
- Wrap works on all four edges; self-collision still ends the game.
- Mode survives unplugging.

**Checklist**
- [x] Walls and wrap both behave correctly
- [x] Settings disabled during a game; mode remembered after power-off
- [x] Committed and tagged v1.1