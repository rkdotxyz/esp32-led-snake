# Parts list

| Part | Qty | Notes |
|---|---|---|
| ESP32 DevKit V1 (30-pin) | 1 | |
| WS2812B 8×32 flexible LED matrix | 1 | 256 LEDs, serpentine wiring |
| Electrolytic capacitor, 100 µF, 25 V | 1 | Across 5 V and GND at the matrix input; stripe side to GND. 1000 µF recommended if moving to an external supply |
| Resistor, 330–470 Ω | 1 | In series on the data line |
| Micro-USB **data** cable | 1 | Charge-only cables won't be detected |
| Breadboard + jumper wires | – | |
| Multimeter | 1 | Recommended |
| USB charger or power bank | 1 | For running away from the laptop |

Power: USB only. Firmware caps LED current at ~300 mA.
Optional later: 5 V 4 A supply + barrel-jack terminal for brighter effects.
