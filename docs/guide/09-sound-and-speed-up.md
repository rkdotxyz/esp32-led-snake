# Phase 9: Sound and speed-up (v2.1)

**Goal:** the phone plays sounds for game moments, and the snake gets faster as it grows. No wiring changes; three files change.

**Code at the end of this phase:** [`v2.1`](https://github.com/rkdotxyz/esp32-led-snake/tree/v2.1)

## Speed-up

Each food takes 5 ms off the time between steps, from 250 ms down to a floor of 120 ms, reached after 26 foods:

```cpp
unsigned long currentTickMs() {
  unsigned long speedup = (unsigned long)gameScore() * SPEEDUP_PER_FOOD_MS;
  unsigned long maxSpeedup = START_TICK_MS - MIN_TICK_MS;
  if (speedup > maxSpeedup) {
    speedup = maxSpeedup;
  }
  return START_TICK_MS - speedup;
}
```

Set `SPEEDUP_PER_FOOD_MS` to 0 in `config.h` to turn it off.

## Events, not just status

Until now the ESP32 only told phones how things *are*: score, state, settings. Sounds need *moments*, so there's a second kind of message:

```
event start    event eat    event crash
```

Status is re-sent to every newly connected phone; events are sent once. A phone joining mid-game gets the current score but never replays an old crash sound.

## Sounds made from scratch

No audio files. The Web Audio API builds each sound from a wave that slides between two pitches while fading out:

```js
function tone(type, fromHz, toHz, seconds, delay = 0, volume = 0.15) {
  const start = audio.currentTime + delay;
  const oscillator = audio.createOscillator();
  const gain = audio.createGain();
  oscillator.type = type;                                   // 'square', 'sawtooth', 'triangle'
  oscillator.frequency.setValueAtTime(fromHz, start);
  oscillator.frequency.exponentialRampToValueAtTime(toHz, start + seconds);
  gain.gain.setValueAtTime(volume, start);
  gain.gain.exponentialRampToValueAtTime(0.001, start + seconds);
  oscillator.connect(gain).connect(audio.destination);
  oscillator.start(start);
  oscillator.stop(start + seconds);
}

const sounds = {
  eat: () => tone('square', 660, 1320, 0.09, 0, 0.1),      // quick upward blip
  crash: () => tone('sawtooth', 320, 60, 0.5, 0, 0.18),    // falling buzz
};
```

Browsers only allow sound after the user has touched the page, so the audio engine is created on the first touch or key press. On iPhone, sound follows the silent switch.

Sound and vibration can each be turned off in Settings, under "On this device". Those switches are saved in the browser's localStorage, not on the ESP32, because they're personal to each phone.

## Vibration: an open issue

Android supports `navigator.vibrate()`. Safari on iPhone doesn't. There's a known workaround: toggle a hidden `<input type="checkbox" switch>` from code, which makes an iPhone tap. It's unofficial and version-dependent: reported to work on iOS 17.4 to 26.4, and blocked in 26.5.

**It didn't work on this build's iPhone (iOS 26.3),** even after keeping the hidden switch on-screen and clicking both the switch and its label. The code stays in, since it works on Android and does no harm on iPhone. If you pick it up: first check whether toggling the Settings panel's own Vibration switch by finger makes your phone tap. That confirms the phone's haptic settings are on (Settings → Sounds & Haptics → System Haptics, and Accessibility → Touch → Vibration).

## What to check

- [ ] Starting a game plays two rising notes; eating a blip; crashing a falling buzz.
- [ ] Serial Monitor shows the step time dropping by 5 ms per food, stopping at 120 ms.
- [ ] Sound can be turned off in Settings, and stays off after reloading.

## If something's wrong

| Symptom | Likely cause and fix |
|---|---|
| No sound | Silent switch on, volume down, Sound off in Settings, or no tap on the page yet. |
| "TICK_MS was not declared" | `config.h` and `snake_matrix.ino` must both be updated. |
| No new switches in Settings | Embed script not re-run, or cached page. |

**Next:** [Phase 10: Document and publish](10-document-and-publish.md)
