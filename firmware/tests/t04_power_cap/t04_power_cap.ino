// =====================================================================
// t04_power_cap
// ---------------------------------------------------------------------
// Worst-case power test. Every LED is set to white, the most power-hungry
// colour, and the sketch asks for more brightness every 4 seconds.
//
// The power cap should stop the panel getting brighter after the first
// couple of steps, and the ESP32 should never restart. Serial Monitor
// shows what brightness was requested, what the cap allowed, and
// FastLED's estimate of the current.
//
// Same wiring as t02_first_pixels. Serial Monitor: 115200 baud.
// If you see "BROWNOUT" after a restart, lower MAX_MILLIAMPS.
// =====================================================================

#include <FastLED.h>
#include <esp_system.h>


// ---------- Settings ----------
const int DATA_PIN = 2;               // GPIO2 (D2)
const int NUM_LEDS = 256;             // 32 x 8
const uint32_t MAX_MILLIAMPS = 300;   // the power cap we're testing
const unsigned long STEP_MS = 4000;   // how long each brightness step lasts

// The brightness levels to ask for, in order (0 to 255).
const uint8_t STEPS[] = {5, 10, 20, 40, 80, 160, 255};

// How many entries STEPS has. sizeof gives sizes in bytes, so dividing the
// whole array's size by one entry's size gives the count. If you add or
// remove steps above, this updates itself.
const int NUM_STEPS = sizeof(STEPS) / sizeof(STEPS[0]);


// ---------- LED buffer and state ----------
CRGB leds[NUM_LEDS];
int stepIndex = 0;             // which entry in STEPS we're on
unsigned long lastStep = 0;    // when we last changed step


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("t04_power_cap");
  printResetReason();
  Serial.printf("Power cap: %lu mA\n", (unsigned long)MAX_MILLIAMPS);

  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_MILLIAMPS);

  // fill_solid sets every LED in the buffer to one colour.
  // White means red, green and blue all fully on: the worst case for power.
  fill_solid(leds, NUM_LEDS, CRGB::White);

  applyStep();           // show the first brightness step
  lastStep = millis();
}


void loop() {
  // Same non-blocking pattern as t02: check the clock, act when it's time.
  if (millis() - lastStep >= STEP_MS) {
    lastStep = millis();
    stepIndex++;
    if (stepIndex >= NUM_STEPS) {   // after the last step, start over
      stepIndex = 0;
      Serial.println("-- starting again --");
    }
    applyStep();
  }
}


// Asks for the current step's brightness, then reports what the cap did.
void applyStep() {
  uint8_t requested = STEPS[stepIndex];
  FastLED.setBrightness(requested);  // what we ASK for
  FastLED.show();                    // FastLED applies the cap while sending

  // Ask FastLED the same question its cap asks internally:
  // "given these colours, what's the highest brightness that stays under
  // MAX_MILLIAMPS at 5 volts?"
  uint8_t allowed = calculate_max_brightness_for_power_vmA(leds, NUM_LEDS, requested, 5, MAX_MILLIAMPS);

  // And its estimate of the power at that brightness, in milliwatts.
  // Dividing milliwatts by 5 volts gives milliamps.
  uint32_t milliwatts = scale_power_for_brightness(calculate_unscaled_power_mW(leds, NUM_LEDS), allowed);

  Serial.printf("Requested %3d/255 -> cap allows %3d/255, about %lu mA%s\n",
                requested, allowed, (unsigned long)(milliwatts / 5),
                allowed < requested ? "   (cap active)" : "");
}


// Prints why the ESP32 last started. See t02_first_pixels for details.
void printResetReason() {
  Serial.print("Reset reason: ");
  switch (esp_reset_reason()) {
    case ESP_RST_POWERON:  Serial.println("power on"); break;
    case ESP_RST_EXT:      Serial.println("reset button"); break;
    case ESP_RST_SW:       Serial.println("software restart"); break;
    case ESP_RST_BROWNOUT: Serial.println("BROWNOUT: the LEDs drew more power than USB could supply"); break;
    case ESP_RST_PANIC:    Serial.println("crash (panic)"); break;
    default:               Serial.println("other"); break;
  }
}
