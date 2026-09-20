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
- [ ] Repo pushed to GitHub, tagged v0.1