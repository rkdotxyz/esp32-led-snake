// =====================================================================
// controller_page.h  --  GENERATED FILE, do not edit by hand.
// Source: controller/index.html
// Regenerate with: python3 tools/embed_page.py
// =====================================================================

#pragma once

// R"rawliteral( ... )rawliteral" is a C++ "raw string": everything in
// between is kept exactly as written, so quotes and backslashes in the
// HTML need no escaping.
const char CONTROLLER_PAGE[] = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<!-- Fit the phone screen, stop pinch/double-tap zoom, draw under the notch -->
<meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no, viewport-fit=cover">
<meta name="theme-color" content="#0b0f0d">
<title>Snake controller</title>
<style>
  :root {
    --bg: #0b0f0d;
    --panel: #151b18;
    --line: #27312c;
    --text: #e6ece8;
    --muted: #8b988f;
    --green: #46e07f;
    --red: #ff5a4a;
  }
  * { box-sizing: border-box; -webkit-tap-highlight-color: transparent; }
  html, body { margin: 0; height: 100%; background: var(--bg); color: var(--text); }
  body {
    display: flex;
    flex-direction: column;
    font-family: -apple-system, system-ui, "Segoe UI", Roboto, sans-serif;
    /* keep clear of the notch and home bar on phones */
    padding: max(16px, env(safe-area-inset-top)) 16px max(16px, env(safe-area-inset-bottom));
    touch-action: none;            /* our swipes, not page scrolling */
    user-select: none;
    -webkit-user-select: none;
    -webkit-touch-callout: none;   /* no long-press menu on iPhone */
    overscroll-behavior: none;
  }

  header { display: flex; justify-content: space-between; align-items: center; }
  .conn { display: flex; align-items: center; gap: 8px; color: var(--muted); font-size: 14px; }
  .dot { width: 10px; height: 10px; border-radius: 50%; background: var(--red); }
  .dot.on { background: var(--green); }
  .score { color: var(--muted); font-size: 14px; }
  .score b { color: var(--text); font-size: 28px; margin-left: 6px; font-variant-numeric: tabular-nums; }

  #status { text-align: center; font-size: 18px; min-height: 1.5em; margin: 20px 0 8px; }

  #pad { flex: 1; display: grid; place-items: center; }
  .dpad {
    display: grid;
    grid-template-columns: repeat(3, 1fr);
    grid-template-rows: repeat(3, 1fr);
    gap: 10px;
    width: min(82vw, 46vh, 360px);
    aspect-ratio: 1;
  }
  button {
    font: inherit;
    color: var(--text);
    background: var(--panel);
    border: 1px solid var(--line);
    border-radius: 18px;
    transition: background 80ms, border-color 80ms;
  }
  button.hit { background: #1f2c25; border-color: var(--green); }
  .dpad button { font-size: 30px; }
  .up    { grid-area: 1 / 2; }
  .left  { grid-area: 2 / 1; }
  .right { grid-area: 2 / 3; }
  .down  { grid-area: 3 / 2; }
  .mid {
    grid-area: 2 / 2;
    display: grid; place-items: center; text-align: center;
    color: var(--muted); font-size: 12px; line-height: 1.3;
  }

  .actions { display: flex; gap: 12px; margin-top: 12px; }
  .actions button { flex: 1; padding: 16px; font-size: 17px; border-radius: 14px; }
  button:disabled { opacity: 0.35; }

  /* Settings panel: a dark backdrop with a card anchored to the bottom */
  .sheet {
    position: fixed; inset: 0;
    background: rgba(0, 0, 0, 0.6);
    display: flex; align-items: flex-end; justify-content: center;
    padding: 16px 16px max(16px, env(safe-area-inset-bottom));
  }
  .sheet[hidden] { display: none; }
  .sheet-card {
    width: min(100%, 480px);
    max-height: 88vh;
    overflow-y: auto;
    touch-action: pan-y;           /* the page blocks scrolling; the card allows it */
    background: var(--panel);
    border: 1px solid var(--line);
    border-radius: 20px;
    padding: 20px;
  }
  .sheet-card h2 { margin: 0 0 16px; font-size: 20px; }
  .setting { display: flex; flex-direction: column; gap: 12px; padding-bottom: 20px; }
  .setting-label b { display: block; font-size: 16px; margin-bottom: 4px; }
  .setting-label span { color: var(--muted); font-size: 14px; }
  .modes { display: flex; gap: 8px; }
  .modes button { flex: 1; padding: 12px; font-size: 16px; border-radius: 12px; }
  .modes button.active { border-color: var(--green); color: var(--green); }
  .swatches { display: flex; flex-wrap: wrap; gap: 12px; }
  .swatch {
    width: 40px; height: 40px;
    border-radius: 50%;
    border: 2px solid var(--line);
    background: var(--c);
    padding: 0;
  }
  .swatch.active { outline: 2px solid var(--text); outline-offset: 3px; }
  /* The last swatch opens the colour picker. It shows a rainbow until a
     custom colour is chosen, then shows that colour. */
  .swatch.custom {
    position: relative;
    overflow: hidden;
    background: conic-gradient(red, yellow, lime, cyan, blue, magenta, red);
  }
  .swatch.custom.filled { background: var(--c); }
  .swatch.custom input {
    position: absolute; inset: 0;
    width: 100%; height: 100%;
    opacity: 0;                    /* invisible, but still tappable */
    border: 0; padding: 0;
  }
  .toggle {
    display: flex; justify-content: space-between; align-items: center;
    font-size: 16px; padding: 6px 0;
  }
  .toggle input { width: 22px; height: 22px; accent-color: var(--green); }
  .note { color: var(--muted); font-size: 13px; margin: 0 0 16px; }
  .done { width: 100%; padding: 14px; font-size: 17px; border-radius: 14px; }
  .hint { text-align: center; color: var(--muted); font-size: 13px; margin: 12px 0 0; }
  /* touch screens don't need the keyboard hint */
  @media (hover: none) { .hint { display: none; } }
</style>
</head>
<body>

<header>
  <div class="conn"><span class="dot" id="dot"></span><span id="conn">Connecting…</span></div>
  <div class="score">Score<b id="score">0</b></div>
</header>

<div id="status"></div>

<div id="pad">
  <div class="dpad">
    <button class="up" data-dir="up" aria-label="Up">▲</button>
    <button class="left" data-dir="left" aria-label="Left">◀</button>
    <div class="mid">or swipe<br>anywhere</div>
    <button class="right" data-dir="right" aria-label="Right">▶</button>
    <button class="down" data-dir="down" aria-label="Down">▼</button>
  </div>
</div>

<div class="actions">
  <button id="pause">Pause</button>
  <button id="restart">Restart</button>
  <button id="open-settings">Settings</button>
</div>
<p class="hint">Keyboard: arrow keys or W A S D · R restart · P or Space pause</p>

<!-- Settings: a panel that slides over the controller. Only available
     between games; the button is disabled while a game is on. -->
<div id="settings" class="sheet" hidden>
  <div class="sheet-card" role="dialog" aria-modal="true" aria-labelledby="settings-title">
    <h2 id="settings-title">Settings</h2>

    <div class="setting">
      <div class="setting-label">
        <b>Edges</b>
        <span>Walls end the game. Wrap brings you back in on the opposite side.</span>
      </div>
      <div class="modes" role="group" aria-label="Edges">
        <button data-mode="walls">Walls</button>
        <button data-mode="wrap">Wrap</button>
      </div>
    </div>

    <div class="setting">
      <div class="setting-label"><b>Snake colour</b></div>
      <div class="swatches" id="snake-swatches"></div>
    </div>

    <div class="setting">
      <div class="setting-label"><b>Food colour</b></div>
      <div class="swatches" id="food-swatches"></div>
    </div>

    <div class="setting">
      <div class="setting-label">
        <b>On this device</b>
        <span>Saved on this phone or laptop only. Sound follows your phone's silent switch.</span>
      </div>
      <label class="toggle">Sound <input type="checkbox" switch id="pref-sound"></label>
      <label class="toggle">Vibration <input type="checkbox" switch id="pref-haptics"></label>
    </div>

    <p class="note">The matrix shows every colour at full strength, so dark picks come out brighter, and LEDs look a little different from your screen.</p>

    <button id="close-settings" class="done">Done</button>
  </div>
</div>

<script>
  // ---------- Page elements ----------
  const dot = document.getElementById('dot');
  const connText = document.getElementById('conn');
  const scoreEl = document.getElementById('score');
  const statusEl = document.getElementById('status');
  const pauseBtn = document.getElementById('pause');
  const restartBtn = document.getElementById('restart');
  const settingsBtn = document.getElementById('open-settings');
  const settingsSheet = document.getElementById('settings');
  const closeSettingsBtn = document.getElementById('close-settings');

  // ---------- Colour choices ----------
  // Lower-case hex, at full brightness, so they match exactly what the
  // ESP32 sends back after it normalises a colour.
  const PRESETS = {
    snake: [
      ['#00ff40', 'Green'], ['#00e5ff', 'Cyan'], ['#2060ff', 'Blue'],
      ['#8a2bff', 'Purple'], ['#ff2d95', 'Pink'], ['#ffd000', 'Yellow'],
    ],
    food: [
      ['#ff2000', 'Red'], ['#ff7a00', 'Orange'], ['#ffd000', 'Yellow'],
      ['#ff2d95', 'Pink'], ['#ffffff', 'White'],
    ],
  };

  // ---------- Per-device preferences ----------
  // Sound and vibration are personal, so they're saved in this browser
  // (localStorage), not on the ESP32. try/catch because storage can be
  // unavailable, for example in private browsing.
  function loadPref(name, fallback) {
    try {
      const value = localStorage.getItem(`snake-${name}`);
      return value === null ? fallback : value === '1';
    } catch (e) {
      return fallback;
    }
  }
  function savePref(name, on) {
    try { localStorage.setItem(`snake-${name}`, on ? '1' : '0'); } catch (e) {}
  }
  const prefs = {
    sound: loadPref('sound', true),
    haptics: loadPref('haptics', true),
  };

  // ---------- Sound ----------
  // Sounds are made on the spot with the Web Audio API: no sound files.
  // Browsers only allow audio after the user has touched the page, so
  // the audio engine is created (or woken) on the first touch or key.
  let audio = null;

  function unlockAudio() {
    if (!audio) {
      const AudioEngine = window.AudioContext || window.webkitAudioContext;
      if (!AudioEngine) return;
      audio = new AudioEngine();
    }
    if (audio.state === 'suspended') audio.resume();
  }
  // `true` = run before any other handler, so audio is ready in time.
  document.addEventListener('pointerdown', unlockAudio, true);
  document.addEventListener('keydown', unlockAudio, true);

  // One note: a wave that slides from one pitch to another while fading
  // out. type is the wave's shape: 'square' sounds retro, 'sawtooth'
  // buzzy, 'triangle' soft. Pitch is in Hz (440 = the A above middle C).
  function tone(type, fromHz, toHz, seconds, delay = 0, volume = 0.15) {
    if (!audio || !prefs.sound) return;
    const start = audio.currentTime + delay;
    const oscillator = audio.createOscillator();
    const gain = audio.createGain();
    oscillator.type = type;
    oscillator.frequency.setValueAtTime(fromHz, start);
    oscillator.frequency.exponentialRampToValueAtTime(toHz, start + seconds);
    gain.gain.setValueAtTime(volume, start);
    gain.gain.exponentialRampToValueAtTime(0.001, start + seconds);  // fade out
    oscillator.connect(gain).connect(audio.destination);
    oscillator.start(start);
    oscillator.stop(start + seconds);
  }

  const sounds = {
    start: () => {                                  // two rising notes
      tone('triangle', 523, 523, 0.08);
      tone('triangle', 784, 784, 0.12, 0.09);
    },
    eat: () => tone('square', 660, 1320, 0.09, 0, 0.1),     // quick upward blip
    crash: () => tone('sawtooth', 320, 60, 0.5, 0, 0.18),   // falling buzz
  };

  // ---------- Vibration ----------
  // Android: navigator.vibrate() works directly.
  // iPhone: Safari doesn't support it. The workaround: a hidden switch
  // control (<input type="checkbox" switch>) makes the iPhone tap when it's
  // toggled, so we toggle it from code. Apple blocked this in iOS 26.5; on
  // newer iOS it silently does nothing.
    const hapticLabel = document.createElement('label');
  const hapticSwitch = document.createElement('input');
  hapticSwitch.type = 'checkbox';
  hapticSwitch.setAttribute('switch', '');
  hapticLabel.appendChild(hapticSwitch);
  hapticLabel.setAttribute('aria-hidden', 'true');
  // Invisible, but still on-screen and laid out: iOS ignores taps on
  // elements parked off-screen or hidden with display:none.
  hapticLabel.style.cssText =
    'position:fixed;bottom:0;left:0;width:1px;height:1px;opacity:0.01;pointer-events:none;z-index:-1';
  document.body.appendChild(hapticLabel);

  // pulses: 1 = a single tap, 3 = three quick taps (for a crash)
  function haptic(pulses = 1) {
    if (!prefs.haptics) return;
    if (navigator.vibrate) {
      navigator.vibrate(pulses === 1 ? 12 : [30, 60, 30, 60, 30]);
      return;
    }
    for (let i = 0; i < pulses; i++) {
      setTimeout(() => {
        hapticSwitch.click();   // toggling the switch is what makes iOS tap
        hapticLabel.click();    // some iOS versions only respond to the label
      }, i * 90);
    }
  }

  // Game moments from the ESP32 become sound and vibration.
  function handleEvent(name) {
    if (name === 'start') { sounds.start(); }
    if (name === 'eat')   { sounds.eat(); haptic(1); }
    if (name === 'crash') { sounds.crash(); haptic(3); }
  }

  // ---------- Game state, as told to us by the ESP32 ----------
  let state = 'ready';
  let reason = '';
  let mode = 'walls';
  const statusText = {
    ready: 'Press any direction to start',
    playing: '',
    paused: 'Paused. Any direction resumes',
  };

  // ---------- The live connection ----------
  let ws = null;

  function connect() {
    // Opened from a file on your computer rather than from the ESP32?
    // Then there's nothing to connect to, but the layout still shows.
    if (!location.host) {
      connText.textContent = 'Open from the ESP32 to play';
      return;
    }
    // location.host is whatever address you typed (snake.local or 192.168.4.1),
    // so the socket always goes back to the same ESP32 that served the page.
    ws = new WebSocket(`ws://${location.host}/ws`);
    ws.onopen = () => setConnected(true);
    ws.onclose = () => {
      setConnected(false);
      setTimeout(connect, 1000);   // keep retrying every second
    };
    ws.onmessage = (event) => handleMessage(event.data);
  }

  function setConnected(on) {
    dot.classList.toggle('on', on);
    connText.textContent = on ? 'Connected' : 'Reconnecting…';
  }

  // Sends one command word, with a light tap of vibration.
  function send(command) {
    if (ws && ws.readyState === WebSocket.OPEN) {
      ws.send(command);
    }
    haptic(1);                     // a light tap under your finger
  }

  // Messages look like "score 7" or "state paused": a key, a space, a value.
  function handleMessage(message) {
    const space = message.indexOf(' ');
    const key = space < 0 ? message : message.slice(0, space);
    const value = space < 0 ? '' : message.slice(space + 1);

    if (key === 'score') scoreEl.textContent = value;
    if (key === 'reason') reason = value;
    if (key === 'mode') showMode(value);
    if (key === 'snake' || key === 'food') showColour(key, value);
    if (key === 'event') handleEvent(value);
    if (key === 'state') {
      state = value;
      showState();
    }
  }

  function showState() {
    statusEl.textContent = state === 'over'
      ? `Game over: ${reason}. Any direction plays again`
      : (statusText[state] || '');
    pauseBtn.textContent = state === 'paused' ? 'Resume' : 'Pause';
    updateSettingsButton();
  }

  // ---------- Settings panel ----------
  // Settings only change between games. If a game starts while the panel
  // is open (say, from a second phone), the panel closes itself.
  function gameInProgress() {
    return state === 'playing' || state === 'paused';
  }

  function updateSettingsButton() {
    settingsBtn.disabled = gameInProgress();
    if (gameInProgress()) closeSettings();
  }

  function openSettings() {
    if (gameInProgress()) return;
    settingsSheet.hidden = false;
  }

  function closeSettings() {
    settingsSheet.hidden = true;
  }

  function settingsOpen() {
    return !settingsSheet.hidden;
  }

  // Highlights the current edge mode. The page only changes it once the
  // ESP32 confirms, so it can never show a mode the game isn't using.
  function showMode(newMode) {
    mode = newMode;
    document.querySelectorAll('[data-mode]').forEach((button) => {
      button.classList.toggle('active', button.dataset.mode === mode);
    });
  }

  // Builds one row of colour swatches: the presets, then a custom one
  // that opens the device's colour picker. `target` is 'snake' or 'food'.
  function buildSwatches(target) {
    const row = document.getElementById(`${target}-swatches`);

    PRESETS[target].forEach(([hex, name]) => {
      const button = document.createElement('button');
      button.className = 'swatch';
      button.style.setProperty('--c', hex);
      button.dataset.hex = hex;
      button.setAttribute('aria-label', name);
      button.addEventListener('pointerdown', (event) => {
        event.preventDefault();
        send(`${target} ${hex}`);
      });
      row.appendChild(button);
    });

    const custom = document.createElement('label');
    custom.className = 'swatch custom';
    custom.setAttribute('aria-label', 'Custom colour');
    const picker = document.createElement('input');
    picker.type = 'color';
    // 'change' fires once, when the picker is closed or confirmed. ('input'
    // would fire continuously while dragging, and each one would be saved
    // to the ESP32's flash.)
    picker.addEventListener('change', () => send(`${target} ${picker.value}`));
    custom.appendChild(picker);
    row.appendChild(custom);
  }

  // Marks the swatch matching the ESP32's colour. If it isn't a preset,
  // the custom swatch lights up and shows it.
  function showColour(target, hex) {
    const row = document.getElementById(`${target}-swatches`);
    let matched = false;
    row.querySelectorAll('button.swatch').forEach((button) => {
      const isThis = button.dataset.hex === hex;
      button.classList.toggle('active', isThis);
      if (isThis) matched = true;
    });
    const custom = row.querySelector('.custom');
    custom.classList.toggle('active', !matched);
    custom.classList.toggle('filled', !matched);
    custom.style.setProperty('--c', hex);
    custom.querySelector('input').value = hex;
  }

  // Briefly highlights a button, so every press gets visible feedback.
  function flash(button) {
    button.classList.add('hit');
    setTimeout(() => button.classList.remove('hit'), 120);
  }

  // ---------- On-screen buttons ----------
  // pointerdown fires the instant a finger touches the screen, faster
  // than 'click', which waits for the finger to lift.
  document.querySelectorAll('[data-dir]').forEach((button) => {
    button.addEventListener('pointerdown', (event) => {
      event.preventDefault();
      send(button.dataset.dir);
      flash(button);
    });
  });
  document.querySelectorAll('[data-mode]').forEach((button) => {
    button.addEventListener('pointerdown', (event) => {
      event.preventDefault();
      send(`mode ${button.dataset.mode}`);
      flash(button);
    });
  });
  settingsBtn.addEventListener('pointerdown', (e) => { e.preventDefault(); openSettings(); });
  closeSettingsBtn.addEventListener('pointerdown', (e) => { e.preventDefault(); closeSettings(); });
  // Tapping the dark backdrop (outside the card) also closes it.
  settingsSheet.addEventListener('pointerdown', (e) => {
    if (e.target === settingsSheet) closeSettings();
  });
  pauseBtn.addEventListener('pointerdown', (e) => { e.preventDefault(); send('pause'); flash(pauseBtn); });
  restartBtn.addEventListener('pointerdown', (e) => { e.preventDefault(); send('restart'); flash(restartBtn); });

  // ---------- Swipes ----------
  // Track a finger that starts anywhere except on a button. Once it has
  // moved 30 pixels, send that direction and start measuring again from
  // there, so one continuous drag can make several turns.
  const SWIPE_DISTANCE = 30;
  let swipeStart = null;

  document.addEventListener('pointerdown', (event) => {
    if (settingsOpen()) return;                // no steering behind the panel
    if (event.target.closest('button')) return;
    swipeStart = { x: event.clientX, y: event.clientY };
  });

  document.addEventListener('pointermove', (event) => {
    if (!swipeStart) return;
    const dx = event.clientX - swipeStart.x;
    const dy = event.clientY - swipeStart.y;
    if (Math.max(Math.abs(dx), Math.abs(dy)) < SWIPE_DISTANCE) return;

    // Whichever way moved further wins.
    const direction = Math.abs(dx) > Math.abs(dy)
      ? (dx > 0 ? 'right' : 'left')
      : (dy > 0 ? 'down' : 'up');
    send(direction);
    swipeStart = { x: event.clientX, y: event.clientY };
  });

  document.addEventListener('pointerup', () => { swipeStart = null; });
  document.addEventListener('pointercancel', () => { swipeStart = null; });

  // ---------- Keyboard (laptops) ----------
  const keyCommands = {
    ArrowUp: 'up', w: 'up',
    ArrowDown: 'down', s: 'down',
    ArrowLeft: 'left', a: 'left',
    ArrowRight: 'right', d: 'right',
    r: 'restart',
    p: 'pause', ' ': 'pause',
  };

  document.addEventListener('keydown', (event) => {
    if (event.repeat) return;                  // ignore auto-repeat while a key is held
    const key = event.key.length === 1 ? event.key.toLowerCase() : event.key;
    if (settingsOpen()) {                      // while the panel is open, only Escape works
      if (key === 'Escape') closeSettings();
      return;
    }
    const command = keyCommands[key];
    if (!command) return;
    event.preventDefault();                    // stop arrows and space scrolling the page
    send(command);
    const button = document.querySelector(`[data-dir="${command}"]`);
    if (button) flash(button);
  });

  // ---------- Sound and vibration switches ----------
  [['pref-sound', 'sound'], ['pref-haptics', 'haptics']].forEach(([id, name]) => {
    const toggle = document.getElementById(id);
    toggle.checked = prefs[name];
    toggle.addEventListener('change', () => {
      prefs[name] = toggle.checked;
      savePref(name, toggle.checked);
    });
  });

  // ---------- Start ----------
  buildSwatches('snake');
  buildSwatches('food');
  showState();
  showMode(mode);
  connect();
</script>
</body>
</html>
)rawliteral";
