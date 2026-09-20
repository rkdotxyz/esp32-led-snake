# Gotchas

Carried over from the first Snake build, plus anything new found during this one.

## Hardware
- **Data pin.** GPIO2 drove the LEDs where GPIO13 didn't. GPIO2 is also a boot pin tied to the onboard LED: if uploads fail with the matrix connected, unplug the data wire during upload or move to GPIO16.
- **Serpentine wiring.** The matrix is wired in a zigzag, so (x, y) needs a mapping function to find the LED's index on the strip.
- **Brownouts.** On USB power, too many bright LEDs make the ESP32 reset with `Brownout detector was triggered` in Serial Monitor. That's power, not code.
- **Split breadboard rails.** Many long breadboards split each power rail into two halves around column 30. Check for a break in the red and blue lines. If power goes into one half and the matrix connects to the other, the code runs but nothing lights.

## Software
- **Web server libraries.** The standard WebServer/WebSocketsServer clashed with esp32 board package 3.x. AsyncTCP + ESPAsyncWebServer fixed it. Install both from the same maintainer (ESP32Async); mixing sources causes compile errors.
- **WiFi.** The ESP32 only does 2.4 GHz. This build avoids the issue by hosting its own hotspot.
- **Responsiveness.** Read input separately from the game tick; never use `delay()` in the main loop.
- **Typed commands ignored.** Sketches that read commands from Serial Monitor wait for Enter. Set the line-ending dropdown next to the baud rate to "New Line".
- **Very dim colours disappear.** Every colour is scaled by `BRIGHTNESS` (40/255), so values below about 7 round down to fully off.
- **Dithering shimmers at low brightness.** FastLED flickers dim pixels between two levels to fake in-between shades. Switched off with `FastLED.setDither(DISABLE_DITHER)`.
- **Custom types in function signatures need a header.** Functions in the `.ino` that take `AppState` only compile because the enum lives in `app_state.h` (see the OLED project's auto-prototype problem).

## Arduino IDE
- **Library Manager looks empty** while a board package is installing. Wait for the install to finish, then search again.
- **Wrong board selected.** The IDE remembers the last board (e.g. an Arduino UNO from another project). Check the board dropdown and port before every first upload in a session.

## Serial and terminals
- **Only one program can use the USB port.** Close Arduino IDE's Serial Monitor before using a terminal tool, and quit the terminal tool before uploading.
- **Terminal tools can hold the ESP32 in reset.** Opening the port may toggle the DTR/RTS lines wired to the ESP32's reset and boot pins. The terminal looks connected but the sketch isn't running. Arduino's Serial Monitor handles this correctly; `screen` and `tio` may not.
- **VS Code shows false errors on Arduino code.** It doesn't know where the Arduino libraries live. Trust Arduino IDE's compile result instead.

## WiFi and the controller
- **"No Internet Connection" on iPhone is normal.** The ESP32's hotspot has no internet. Stay connected.
- **A Mac on the hotspot loses internet.** It joins one WiFi network at a time. Switch back when done; Serial Monitor still works over USB.
- **Type `http://`.** Some browsers try `https://` first, which the ESP32 doesn't serve. If `snake.local` fails, use `http://192.168.4.1`.
- **Re-run the embed script after editing the page.** `python3 tools/embed_page.py` from the repo root, then re-upload. Editing `controller_page.h` by hand gets overwritten.
- **Don't name a file `network.h`.** The ESP32 core has its own `Network.h`, and macOS treats the two names as the same file.
- **Save before running.** VS Code's unsaved dot on a tab means the file on disk is still empty or old.