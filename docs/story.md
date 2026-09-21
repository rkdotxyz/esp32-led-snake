# The story of the build

How ESP32 LED Snake went from an unfinished weekend project to a documented v2.1, and what I learned on the way.

## Context

I'm starting an MSc in Interactive Media Technology at KTH, focused on tangible interfaces and physical interaction. I come from design and frontend engineering, so screens and code are familiar; wires, power supplies and microcontrollers are not.

A few months earlier I'd built a first version of Snake on an ESP32 and an 8×32 LED matrix, controlled from a browser. It worked, but it never felt finished, and I couldn't have explained half of why it worked. So I rebuilt it from scratch with a different goal: **understand every part, and leave a repo someone else could rebuild from.**

## The approach

The project was split into ten phases, each adding one thing and ending with something I could see on the matrix. Every phase was committed and tagged (`v0.1` … `v2.1`), so each tag is a working snapshot. That rhythm (small step, visible check, commit) is what kept a first hardware project from turning into a pile of half-working code.

Hardware bring-up came first, with tiny single-purpose test sketches: blink the onboard LED, light one pixel, walk a dot through all 256 LEDs, find out how the matrix is wired, stress-test the power. Only then did the game start.

## Key decisions

**The phone is the controller.** Instead of buttons, the ESP32 hosts its own WiFi network and serves a controller page: a d-pad, swipes, pause, restart and settings. There's no app to install and no router needed, so it works anywhere, including on university networks that an ESP32 can't easily join.

**Designed around a power budget.** Everything runs from one USB cable, roughly 500 mA for the whole board, while the matrix could draw 15 A fully lit. A firmware power cap dims anything that would exceed 300 mA, and the visual design follows from it: a few bright pixels on a dark board, never a lit screen. The constraint shaped the look, not just the wiring.

**The rules know nothing about LEDs or WiFi.** The game module only knows about a grid, a snake and a direction. Because of that, the phone controller, animations, colour themes and speed-up were all added later without changing the rules.

**The ESP32 is the single source of truth.** The page never assumes a setting changed. Tapping a colour sends a request, and the swatch only lights up when the ESP32 confirms. Two phones can't disagree, and a dark colour picked on the phone is shown back at the brightness the matrix actually uses.

**Settings only between games.** The first version let you switch walls and wrap-around mid-game. Changing the rules mid-play felt wrong, so settings moved into a panel that's only available between games, enforced on the ESP32 as well as the page.

## Designing the details

Several changes only became obvious once things ran on the real matrix:

- The attract screen originally lit every LED faintly behind the demo snake. Just the snake and apples on a dark board looked much cleaner.
- The score first used a 7-row font, top-aligned, leaving the bottom row empty. A custom 5×6 font sits centred with the top and bottom rows free.
- Counting up to a two-digit score, single digits looked right-aligned. Counting with leading zeros (00, 01 … 12) keeps the number steady.
- Pick a navy snake and it nearly disappears on the matrix. Colours are now normalised to full strength, with brightness controlled in one place.
- Effects looked choppy when the matrix only redrew as the snake moved. Drawing now runs on its own 30 ms clock, separate from movement.

## Problems solved

**The code ran, but nothing lit up.** Serial Monitor showed the test running perfectly; the matrix stayed dark. The breadboard's power rails were split in the middle, and the ESP32 and the matrix were on different halves. The lesson: when the code says it's working and the hardware disagrees, suspect power first.

**Keyboard play from a terminal.** Serial Monitor needs Enter after every key, so I tried terminal tools that send keypresses instantly. They connected but the game ignored them, most likely because opening the port toggles the ESP32's reset lines. Rather than sink time into it, I brought the browser controller forward a phase, which made the problem irrelevant.

**Two tasks, one queue.** The web server runs in its own background task, so a phone can add a direction at the same instant the game loop takes one out. The shared queue is guarded by a critical section. It's invisible when it works, and very hard to debug when it doesn't.

**A file name clash.** A module called `network.h` would have collided with the ESP32 core's own `Network.h`, because macOS treats the two names as the same file. It became `web_control`.

**Still open: iPhone vibration.** Safari has no vibration API. A known workaround (toggling a hidden switch control) is unofficial and didn't fire on my iPhone. Sound carries the feedback for now.

## What I learned

**Hardware.** Power budgets decide more than wiring. Symptoms have meanings: a brownout message means current, flicker usually means ground or data, total darkness usually means power never arrived. And small test sketches are the fastest debugging tool there is.

**Software.** State machines instead of flags. Timing with `millis()` instead of `delay()`, and separate clocks for moving and drawing. Modules with one job each. What happens when two tasks share data.

**Process.** Tagging every phase made each step safe to try. Writing the build log while things were fresh made this documentation possible.

## What's next

A saved high score, a 3D-printed diffuser so each LED reads as a crisp square pixel, and possibly two-player Snake with two phones. And the one that got away: getting an iPhone to buzz when the snake crashes.

---

The full step-by-step guide is in [`docs/guide`](guide/README.md).
