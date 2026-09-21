# Controller protocol

The controller page and the ESP32 talk over a WebSocket at `ws://<address>/ws`, opened after the page itself is loaded over HTTP from `/`. Every message is a short line of plain text: a word, optionally followed by a space and a value. Plain words make the traffic easy to read and debug.

## Phone → ESP32

| Message | Meaning | When it's accepted |
|---|---|---|
| `up` `down` `left` `right` | Queue a turn. Also starts a game from Attract or Score, and resumes from Paused | Any time (reversing into yourself is ignored) |
| `pause` | Toggle pause | While playing or paused |
| `restart` | Return to the attract screen | Any time |
| `mode walls` / `mode wrap` | Set the edge rule | Between games only |
| `snake #rrggbb` | Set the snake colour | Between games only |
| `food #rrggbb` | Set the food colour | Between games only |

Anything else, including malformed colours, is ignored. Messages longer than 15 characters are cut off before being read.

## ESP32 → phone

**Status** describes how things are right now. The full set is sent whenever anything changes, and to every newly connected phone:

| Message | Example |
|---|---|
| `score <n>` | `score 7` |
| `mode walls` / `mode wrap` | `mode wrap` |
| `snake #rrggbb` | `snake #00ff40` |
| `food #rrggbb` | `food #ff2000` |
| `reason <text>` | `reason hit a wall` (only when the game is over; sent before `state over`) |
| `state <name>` | `ready`, `playing`, `paused` or `over` |

**Events** are one-off moments, sent once and never repeated to late joiners:

| Message | When | The page plays |
|---|---|---|
| `event start` | A game begins | Two rising notes |
| `event eat` | Food eaten | A blip and one vibration tap |
| `event crash` | The snake dies | A falling buzz and three vibration taps |

## A short session

```
page  → ESP32   (connects)
ESP32 → page    score 0 · mode walls · snake #00ff40 · food #ff2000 · state ready
page  → ESP32   right
ESP32 → page    score 0 · … · state playing
ESP32 → page    event start
ESP32 → page    event eat
ESP32 → page    score 1 · … · state playing
ESP32 → page    event crash
ESP32 → page    score 1 · … · reason hit a wall · state over
```

## Notes

- **Settings are confirmed, not assumed.** After a settings request, the page waits for the ESP32 to send the value back before highlighting it. A rejected request (for example mid-game) is answered with the unchanged value.
- **Colours are normalised.** The ESP32 raises a colour's strongest channel to `ff` while keeping its hue (`#336699` → `#55aaff`) and replies with that.
- **Disconnects.** If the last phone disconnects while playing, the game pauses. The page reconnects every second on its own.
