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
</div>
<p class="hint">Keyboard: arrow keys or W A S D · R restart · P or Space pause</p>

<script>
  // ---------- Page elements ----------
  const dot = document.getElementById('dot');
  const connText = document.getElementById('conn');
  const scoreEl = document.getElementById('score');
  const statusEl = document.getElementById('status');
  const pauseBtn = document.getElementById('pause');
  const restartBtn = document.getElementById('restart');

  // ---------- Game state, as told to us by the ESP32 ----------
  let state = 'ready';
  let reason = '';
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

  // Sends one command word. Short vibration where supported (Android).
  function send(command) {
    if (ws && ws.readyState === WebSocket.OPEN) {
      ws.send(command);
    }
    if (navigator.vibrate) navigator.vibrate(8);
  }

  // Messages look like "score 7" or "state paused": a key, a space, a value.
  function handleMessage(message) {
    const space = message.indexOf(' ');
    const key = space < 0 ? message : message.slice(0, space);
    const value = space < 0 ? '' : message.slice(space + 1);

    if (key === 'score') scoreEl.textContent = value;
    if (key === 'reason') reason = value;
    if (key === 'state') {
      state = value;
      showState();
    }
  }

  function showState() {
    statusEl.textContent = state === 'over'
      ? `Game over: ${reason}. Tap Restart`
      : (statusText[state] || '');
    pauseBtn.textContent = state === 'paused' ? 'Resume' : 'Pause';
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
  pauseBtn.addEventListener('pointerdown', (e) => { e.preventDefault(); send('pause'); flash(pauseBtn); });
  restartBtn.addEventListener('pointerdown', (e) => { e.preventDefault(); send('restart'); flash(restartBtn); });

  // ---------- Swipes ----------
  // Track a finger that starts anywhere except on a button. Once it has
  // moved 30 pixels, send that direction and start measuring again from
  // there, so one continuous drag can make several turns.
  const SWIPE_DISTANCE = 30;
  let swipeStart = null;

  document.addEventListener('pointerdown', (event) => {
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
    const command = keyCommands[key];
    if (!command) return;
    event.preventDefault();                    // stop arrows and space scrolling the page
    send(command);
    const button = document.querySelector(`[data-dir="${command}"]`);
    if (button) flash(button);
  });

  // ---------- Start ----------
  showState();
  connect();
</script>
</body>
</html>
)rawliteral";
