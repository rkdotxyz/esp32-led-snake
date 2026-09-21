# Build guide

A step-by-step guide to building ESP32 LED Snake from scratch: from installing the tools to a game you play from your phone, with colours, effects and sound.

It's written for someone new to physical computing. Each phase adds one thing, ends with a check you can see on the matrix, and includes what went wrong the first time it was built, and how it was fixed.

## What you need

**Hardware** (full list in [`hardware/bom.md`](../../hardware/bom.md))

- ESP32 DevKit V1 (30-pin)
- WS2812B 8×32 flexible LED matrix
- 100–1000 µF electrolytic capacitor, 10 V or higher
- 330–470 Ω resistor
- Breadboard, jumper wires, a micro-USB **data** cable

**Software**

- Arduino IDE 2.x, with the **esp32** board package by Espressif
- Libraries: **FastLED**, **Async TCP** and **ESP Async WebServer** (both by ESP32Async)
- Git and a GitHub account; VS Code is handy for everything that isn't the sketch
- Python 3 (phase 5 onwards, for a small helper script)

Built and tested with Arduino IDE 2.3.10, esp32 3.3.11, FastLED 3.10.5, Async TCP 3.5.0 and ESP Async WebServer 3.12.1.

## The phases

| # | Phase | You end up with | Code at this stage |
|---|---|---|---|
| 1 | [Tools and repo](01-tools-and-repo.md) | Tools installed, board talking to your computer, repo on GitHub | [`v0.1`](https://github.com/rkdotxyz/esp32-led-snake/tree/v0.1) |
| 2 | [Power and first light](02-power-and-first-light.md) | The matrix wired and lighting up, power cap tested | [`v0.2`](https://github.com/rkdotxyz/esp32-led-snake/tree/v0.2) |
| 3 | [Map the grid](03-map-the-grid.md) | Code that can light any pixel by column and row | [`v0.3`](https://github.com/rkdotxyz/esp32-led-snake/tree/v0.3) |
| 4 | [Snake core](04-snake-core.md) | A playable game, steered from Serial Monitor | [`v0.4`](https://github.com/rkdotxyz/esp32-led-snake/tree/v0.4) |
| 5 | [Browser controller](05-browser-controller.md) | Play from any phone or laptop over the ESP32's own WiFi | [`v0.5`](https://github.com/rkdotxyz/esp32-led-snake/tree/v0.5) |
| 6 | [Attract screen and game over](06-attract-and-game-over.md) | Demo animation, crash animation, score on the matrix | [`v1.0`](https://github.com/rkdotxyz/esp32-led-snake/tree/v1.0) |
| 7 | [Walls and wrap](07-walls-and-wrap.md) | Two edge modes in a Settings panel, saved in flash | [`v1.1`](https://github.com/rkdotxyz/esp32-led-snake/tree/v1.1) |
| 8 | [Colours and effects](08-colours-and-effects.md) | Custom colours, pulsing head, fading tail, sparkle | [`v2.0`](https://github.com/rkdotxyz/esp32-led-snake/tree/v2.0) |
| 9 | [Sound and speed-up](09-sound-and-speed-up.md) | Phone sounds, a snake that speeds up as it grows | [`v2.1`](https://github.com/rkdotxyz/esp32-led-snake/tree/v2.1) |
| 10 | [Document and publish](10-document-and-publish.md) | This guide, the docs and the README | [`main`](https://github.com/rkdotxyz/esp32-led-snake) |

## How to use the code links

Every phase was committed and tagged when it was finished, so each tag is a snapshot of the whole project at that point. Follow along in either of two ways:

- **Browse it:** click the tag in the table above. GitHub shows the repo exactly as it was then.
- **Check it out:** clone the repo, then switch to a phase:

  ```bash
  git clone https://github.com/rkdotxyz/esp32-led-snake.git
  cd esp32-led-snake
  git checkout v0.4        # the code as it was at the end of phase 4
  git checkout main        # back to the latest
  ```

The guide shows the key parts of the code with explanations. The full files, heavily commented, are in the tags.

## Conventions

- Serial Monitor always runs at **115200 baud**.
- "Upload" means pressing Upload in Arduino IDE with **DOIT ESP32 DEVKIT V1** selected as the board.
- Each phase ends with a **What to check** list. Don't move on until everything in it works.
- Commands in `code blocks` run in a terminal, from the repo's root folder, unless stated otherwise.

Also useful along the way: [gotchas](../gotchas.md), the [build log](../build-log.md), the [architecture](../architecture.md) and the [controller protocol](../protocol.md).
