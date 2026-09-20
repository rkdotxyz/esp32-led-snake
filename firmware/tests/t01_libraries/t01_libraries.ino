// t01_libraries
// Compile check only: proves FastLED, AsyncTCP and ESPAsyncWebServer are
// installed and compatible with your board package. Nothing lights up and
// no WiFi starts. If this compiles and uploads, the libraries are good.
//
// Serial Monitor: 115200 baud.

#include <FastLED.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const int NUM_LEDS = 256;
CRGB leds[NUM_LEDS];         // uses FastLED's colour type
AsyncWebServer server(80);   // uses ESPAsyncWebServer (which needs AsyncTCP)

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("t01_libraries: FastLED, AsyncTCP and ESPAsyncWebServer compiled together");
  Serial.printf("FastLED version code: %d\n", FASTLED_VERSION);
}

void loop() {
}
