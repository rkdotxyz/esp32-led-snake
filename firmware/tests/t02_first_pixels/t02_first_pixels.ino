// =====================================================================
// t02_first_pixels
// ---------------------------------------------------------------------
// First light for the matrix. This sketch does two jobs:
//
//   1. Colour test. LED 0 (the first LED after the plug) shows red, then
//      green, then blue. Serial Monitor says which colour it SHOULD be.
//      If they don't match, the colour order is wrong (see addLeds below).
//
//   2. Chase. A single white dot walks through all 256 LEDs in the order
//      they're chained together on the strip. Its path shows how the
//      matrix is wired internally, which phase 3 needs.
//
// Wiring (USB only):
//   ESP32 VIN -> matrix 5V  (red)
//   ESP32 GND -> matrix GND (white or black)
//   ESP32 D2  -> 330-470 ohm resistor -> matrix DIN (green)
//   1000 uF capacitor across 5V and GND, stripe to GND
//
// Serial Monitor: 115200 baud.
// The ESP32's blue onboard LED is also on GPIO2, so it flickers along
// with the data. That's normal.
// =====================================================================

// FastLED is the library that talks to WS2812B LEDs. We fill an array of
// colours in memory; FastLED turns it into the precisely timed signal the
// LEDs understand.
#include <FastLED.h>

// esp_system.h gives us esp_reset_reason(), which tells us WHY the ESP32
// last started, including whether a power dip (a "brownout") reset it.
#include <esp_system.h>


// ---------- Settings ----------
// `const` values never change while the program runs. Keeping them
// together at the top makes them easy to find and tweak later.

const int DATA_PIN = 2;               // GPIO2, labelled D2 on the board
const int NUM_LEDS = 256;             // 32 columns x 8 rows
const uint8_t BRIGHTNESS = 40;        // 0 to 255. 40 is plenty indoors and gentle on USB power
const uint32_t MAX_MILLIAMPS = 300;   // power cap: if a frame would draw more than this,
                                      // FastLED dims everything automatically
const unsigned long COLOUR_MS = 1500; // how long each colour test lasts (milliseconds)
const unsigned long STEP_MS = 40;     // time between chase steps; lower = faster


// ---------- The LED buffer ----------
// One CRGB (a red, green, blue colour) per LED. We change colours in this
// array, then call FastLED.show() to send the whole array to the matrix.
// Nothing changes on the actual LEDs until show() is called.
CRGB leds[NUM_LEDS];


// ---------- State ----------
// Instead of pausing with delay(), the sketch remembers which stage it's in
// and checks the clock every time loop() runs. This is called non-blocking
// timing, and the whole game will be built this way.
int stage = 0;                  // 0 = red test, 1 = green test, 2 = blue test, 3 = chase
unsigned long stageStart = 0;   // when the current colour test began
unsigned long lastStep = 0;     // when the chase dot last moved
int dot = 0;                    // which LED the chase dot is on right now
int lap = 1;                    // how many times the dot has gone round


// setup() runs once, when the board powers on or resets.
void setup() {
  Serial.begin(115200);   // open the USB serial connection to Serial Monitor
  delay(1000);            // one-off pause so Serial Monitor can catch the first lines
                          // (delay is fine in setup; we only avoid it in loop)
  Serial.println();
  Serial.println("t02_first_pixels");
  printResetReason();     // tells us if the last restart was a brownout

  // Tell FastLED what's connected:
  //   WS2812B   = the LED chip type
  //   DATA_PIN  = which ESP32 pin sends the data
  //   GRB       = the order the chip expects colours in. WS2812B is usually
  //               green-red-blue. If the colour test looks wrong, try RGB.
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);

  // The safety net: FastLED estimates each frame's current draw before
  // sending it, and dims the frame if it would exceed this cap.
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_MILLIAMPS);

  FastLED.setBrightness(BRIGHTNESS);  // overall brightness for every frame
  FastLED.clear(true);                // all LEDs off; `true` also sends it to the matrix

  stageStart = millis();  // millis() = milliseconds since the board started
  showColourTest();       // light LED 0 red to begin
}


// loop() runs over and over, thousands of times a second.
void loop() {
  unsigned long now = millis();

  // --- Stages 0 to 2: colour test on LED 0 ---
  if (stage < 3) {
    // "Has COLOUR_MS passed since this stage began?"
    // Subtracting like this (now - start) stays correct even when millis()
    // wraps back to zero after about 49 days.
    if (now - stageStart >= COLOUR_MS) {
      stage++;
      stageStart = now;
      if (stage < 3) {
        showColourTest();   // next colour
      } else {
        startChase();       // colour tests done
      }
    }
    return;  // skip the chase code below until we reach stage 3
  }

  // --- Stage 3: one dot walking along the strip, forever ---
  if (now - lastStep >= STEP_MS) {
    lastStep = now;

    leds[dot] = CRGB::Black;   // switch off the LED the dot was on
    dot++;                     // move to the next LED in the chain
    if (dot >= NUM_LEDS) {     // past the last LED? start again at LED 0
      dot = 0;
      lap++;
      Serial.printf("Lap %d\n", lap);
    }
    leds[dot] = CRGB::White;   // switch on the new position
    FastLED.show();            // send the updated buffer to the matrix

    // Print every 8th LED number. With 8 rows, a new column (or row)
    // likely starts every 8 LEDs, which helps you read the dot's path.
    if (dot % 8 == 0) {
      Serial.printf("LED %d\n", dot);
    }
  }
}


// Lights LED 0 in the colour for the current stage and says what it should be.
void showColourTest() {
  const char* names[] = {"RED", "GREEN", "BLUE"};
  CRGB colours[] = {CRGB::Red, CRGB::Green, CRGB::Blue};

  FastLED.clear();              // everything off in the buffer (not sent yet)
  leds[0] = colours[stage];     // stage is 0, 1 or 2 here
  FastLED.show();               // now send it
  Serial.printf("LED 0 (the first LED after the plug) should be %s\n", names[stage]);
}


// Clears the colour test and puts the dot on LED 0.
void startChase() {
  Serial.println("Chase: one dot walks along all 256 LEDs in strip order");
  FastLED.clear();
  dot = 0;
  leds[dot] = CRGB::White;
  FastLED.show();
  Serial.println("LED 0");
  lastStep = millis();
}


// Prints why the ESP32 last started. "BROWNOUT" means the voltage dipped
// too low, almost always because the LEDs drew more than USB could supply.
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
