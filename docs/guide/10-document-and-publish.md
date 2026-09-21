# Phase 10: Document and publish

**Goal:** make the repo something anyone can understand, rebuild and enjoy.

**Code:** [`main`](https://github.com/rkdotxyz/esp32-led-snake)

## What was done

- **Code tidy-up.** File comments rewritten to describe the finished project instead of the phase that added them; unused code removed; a [README for the test sketches](../../firmware/tests/README.md). No behaviour change.
- **Docs.** A [wiring diagram](../../hardware/wiring.svg), the [architecture](../architecture.md) and the [controller protocol](../protocol.md).
- **This build guide**, rewritten from the working notes for each phase, updated to what actually happened, and linked to each phase's tag.
- **[The story of the build](../story.md)**: goals, decisions, problems solved and what's next.
- **The README**, rewritten as the front page of the project.

## Filming LEDs

Cameras over-expose LEDs into white blobs. Two fixes make a big difference:

1. **Lock exposure and turn it down.** In the iPhone Camera app, press and hold on the matrix until "AE/AF LOCK" appears, then drag the sun icon down until the colours look right.
2. **Film in normal room light, not darkness,** with the phone resting on something steady.

To turn a clip into a README-friendly GIF (under 10 MB):

```bash
brew install ffmpeg
ffmpeg -ss 2 -t 12 -i demo.mov -vf "fps=15,scale=800:-1:flags=lanczos,split[a][b];[a]palettegen[p];[b][p]paletteuse" -loop 0 docs/media/demo.gif
```

`-ss 2` skips the first 2 seconds and `-t 12` keeps 12. Lower `fps` or `scale` if the file is too big.

## Where to go from here

Ideas that didn't make it into this build, roughly from smallest to largest:

- iPhone haptics (see [phase 9](09-sound-and-speed-up.md))
- A saved high score
- A 3D-printed diffuser grid, so each LED reads as a square pixel
- Two-player Snake with two phones
- Over-the-air updates, so it can be flashed without a cable
- A second game on the same matrix and controller

**Back to:** [the guide's contents](README.md)
