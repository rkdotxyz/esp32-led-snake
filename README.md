# ESP32 LED Snake

Classic Snake on a 32×8 WS2812B LED matrix, driven by an ESP32 and played from your phone's browser over the ESP32's own WiFi hotspot. No router, no app: join the network, open the page, play.

> **Status:** Phase 1 complete (tools, libraries and board verified). Next up: wiring the matrix and lighting the first pixels.

<!-- Add a GIF here once v1.0 works: ![Demo](docs/media/demo.gif) -->

## Features

| Release | Features | Status |
|---|---|---|
| v1.0 | Attract screen, core Snake (move, eat, grow, die), game over + score, phone controller over the ESP32's hotspot | Planned |
| v1.1 | Wall mode and wrap-around mode, remembered after power-off | Planned |
| v2.0 | Colour themes, pulsing head, fading tail, eat sparkle | Planned |

## How it works

- The **ESP32** runs the game, drives the LEDs and hosts its own WiFi network.
- Your **phone** joins that network and opens a controller page served by the ESP32.
- Button presses travel over a **WebSocket**, so input feels instant.
- The whole thing runs from a **single USB cable**. The firmware caps LED current so it never draws more than USB can supply.

## Hardware

| Part | Notes |
|---|---|
| ESP32 DevKit V1 (ESP32-D0WD-V3) | USB-serial port shows up as `/dev/cu.usbserial-0001` on macOS |
| WS2812B 8×32 flexible LED matrix | 256 LEDs, serpentine wiring |
| 1000 µF capacitor, 330–470 Ω resistor | Power smoothing and data-line protection |
| Micro-USB data cable | Powers everything |

Full parts list: [`hardware/bom.md`](hardware/bom.md). Wiring diagram: coming in phase 2.

## Software

Built and tested with:

| Tool / library | Version |
|---|---|
| Arduino IDE | 2.3.10 |
| esp32 by Espressif Systems (board package) | 3.3.11 |
| FastLED by Daniel Garcia | 3.10.5 |
| Async TCP by ESP32Async | 3.5.0 |
| ESP Async WebServer by ESP32Async | 3.12.1 |

Other versions may work, but these are known to compile together. Install Async TCP and ESP Async WebServer from the same maintainer (ESP32Async); mixing forks causes compile errors.

### Board settings

In Arduino IDE, choose **Tools > Board > esp32 > DOIT ESP32 DEVKIT V1** and select the ESP32's USB-serial port. Defaults are fine for everything else. Serial Monitor runs at **115200 baud**.

## Build it yourself

1. Install the board package and the three libraries above.
2. Flash `firmware/tests/t01_blink` to check your board and cable work.
3. Flash `firmware/tests/t01_libraries` to check the libraries compile together.
4. Follow the phases in [`docs/build-log.md`](docs/build-log.md) to wire and bring up the matrix.

The main game sketch will live in `firmware/snake_matrix/`.

## Repo layout

```
firmware/snake_matrix/   main sketch (open this in Arduino IDE)
firmware/tests/          small single-purpose sketches used to bring up the hardware
controller/              phone controller page (source)
tools/                   helper scripts
hardware/                parts list, wiring diagram, photos
docs/                    build log, gotchas, media
```

## Roadmap

- [x] Phase 1: Tools, libraries and repo
- [ ] Phase 2: Power and first light
- [ ] Phase 3: Map the grid
- [ ] Phase 4: Snake core, played over Serial
- [ ] Phase 5: Attract screen and game over
- [ ] Phase 6: Phone controller (v1.0)
- [ ] Phase 7: Wall and wrap modes (v1.1)
- [ ] Phase 8: Themes and effects (v2.0)
- [ ] Phase 9: Extras
- [ ] Phase 10: Document and publish

## Build log and gotchas

- [`docs/build-log.md`](docs/build-log.md): what was built in each phase, what broke and how it was fixed.
- [`docs/gotchas.md`](docs/gotchas.md): hardware and software pitfalls worth knowing before you start.

## License

MIT. See [`LICENSE`](LICENSE).