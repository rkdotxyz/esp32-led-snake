# Phase 1: Tools and repo

**Goal:** get the tools installed, prove the ESP32 talks to your computer, and put the project on GitHub. Nothing is wired to the matrix yet.

**Code at the end of this phase:** [`v0.1`](https://github.com/rkdotxyz/esp32-led-snake/tree/v0.1)

## Steps

1. **Update Arduino IDE** to the latest 2.x (Arduino IDE → Check for Updates).
2. **Install the board package.** Open Boards Manager (the board icon in the left sidebar), search `esp32`, and install **esp32 by Espressif Systems**. If it isn't listed, add this under Settings → Additional Boards Manager URLs:
   `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
   It's a large download; wait for it to finish completely.
3. **Choose the board:** Tools → Board → esp32 → **DOIT ESP32 DEVKIT V1**.
4. **Clear out old libraries.** If you've built with an ESP32 before, open `Documents/Arduino/libraries` and move any old `AsyncTCP`, `ESPAsyncWebServer`, `WebSockets` or `FastLED` folders somewhere else. Leftover copies from other authors cause confusing compile errors.
5. **Install three libraries** from Library Manager (the books icon): **FastLED**, **Async TCP** and **ESP Async WebServer**. For the last two, check the author says **ESP32Async**; similar-looking ones from other authors don't mix.
6. **Create the repo folder** with this structure:

   ```
   esp32-led-snake/
   ├── README.md
   ├── LICENSE
   ├── .gitignore
   ├── firmware/
   │   └── tests/
   ├── controller/
   ├── tools/
   ├── hardware/
   └── docs/
   ```

   A minimal `.gitignore`:

   ```gitignore
   .DS_Store
   build/
   *.bin
   *.elf
   *.map
   ```

7. **Write the two test sketches** in `firmware/tests/` (see below), and upload each.
8. **Publish to GitHub.** In VS Code, open the repo folder, go to Source Control, and click **Publish to GitHub**. Then tag the phase:

   ```bash
   git tag -a v0.1 -m "Phase 1: tools and repo"
   git push origin v0.1
   ```

## The test sketches

**`t01_blink`** blinks the ESP32's onboard blue LED and prints the chip details and board-package version, which is handy for recording what you built with. It times the blink with `millis()` rather than `delay()`, the pattern the whole game is built on:

```cpp
void loop() {
  if (millis() - lastToggle >= BLINK_MS) {
    lastToggle = millis();
    ledOn = !ledOn;
    digitalWrite(LED_PIN, ledOn ? HIGH : LOW);
  }
}
```

**`t01_libraries`** doesn't light anything. It just includes FastLED, AsyncTCP and ESPAsyncWebServer together and creates one object from each. If it compiles and uploads, the libraries are installed correctly and compatible with your board package.

Arduino IDE rule worth knowing now: a sketch's folder must have the same name as its `.ino` file. `t01_blink.ino` lives in `firmware/tests/t01_blink/`. Use File → Save As and the IDE creates the folder for you.

## What to check

- [ ] The blue LED blinks, and Serial Monitor (115200 baud) prints the chip model and board package version.
- [ ] `t01_libraries` compiles and uploads.
- [ ] The repo is on GitHub with the `v0.1` tag.

## What went wrong for me

- **Library Manager showed no results.** The board package was still installing in the background. Waiting for it to finish fixed it.
- **The IDE was still set to Arduino UNO** from a previous project. Always check the board and port before the first upload in a session.
- **The GitHub remote got named `First-Commit`** instead of the usual `origin`. Renamed it with `git remote rename First-Commit origin`.

## If something's wrong

| Symptom | Likely cause and fix |
|---|---|
| No port appears | A charge-only USB cable. Swap it for a data cable; try another port or adapter. |
| Upload stuck on "Connecting…" | Hold the **BOOT** button on the ESP32 until the upload percentage starts. |
| Garbage in Serial Monitor | Baud rate isn't 115200. (A burst of garbage right after reset is normal: that's the boot messages at a different speed.) |
| Serial Monitor blank, LED blinks | The sketch printed before the monitor connected. Press **EN** to restart it. |
| Compile errors inside a library | Two copies of a library. The output's "Multiple libraries were found" line names the one being used. |

**Next:** [Phase 2: Power and first light](02-power-and-first-light.md)
