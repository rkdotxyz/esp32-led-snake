# Gotchas

Carried over from the first Snake build, plus anything new found during this one.

## Hardware
- **Data pin.** GPIO2 drove the LEDs where GPIO13 didn't. GPIO2 is also a boot pin tied to the onboard LED: if uploads fail with the matrix connected, unplug the data wire during upload or move to GPIO16.
- **Serpentine wiring.** The matrix is wired in a zigzag, so (x, y) needs a mapping function to find the LED's index on the strip.
- **Brownouts.** On USB power, too many bright LEDs make the ESP32 reset with `Brownout detector was triggered` in Serial Monitor. That's power, not code.

## Software
- **Web server libraries.** The standard WebServer/WebSocketsServer clashed with esp32 board package 3.x. AsyncTCP + ESPAsyncWebServer fixed it. Install both from the same maintainer (ESP32Async); mixing sources causes compile errors.
- **WiFi.** The ESP32 only does 2.4 GHz. This build avoids the issue by hosting its own hotspot.
- **Responsiveness.** Read input separately from the game tick; never use `delay()` in the main loop.

## Arduino IDE
- **Library Manager looks empty** while a board package is installing. Wait for the install to finish, then search again.
- **Wrong board selected.** The IDE remembers the last board (e.g. an Arduino UNO from another project). Check the board dropdown and port before every first upload in a session.