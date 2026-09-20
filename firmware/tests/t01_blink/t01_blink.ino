// t01_blink
// Proves the USB cable, port, board settings and Serial Monitor all work,
// and prints the board-package version so you can record it in the README.
//
// Run this with NOTHING wired to the board.
// Serial Monitor: 115200 baud.

#include <esp_arduino_version.h>

const int LED_PIN = 2;               // onboard blue LED on the ESP32 DevKit V1
const unsigned long BLINK_MS = 500;  // time between toggles

unsigned long lastToggle = 0;
bool ledOn = false;

void setup() {
  Serial.begin(115200);
  delay(1000);  // give Serial Monitor a moment to connect

  pinMode(LED_PIN, OUTPUT);

  Serial.println();
  Serial.println("t01_blink: board is alive");
  Serial.printf("Chip: %s, revision %d, %d cores\n",
                ESP.getChipModel(), ESP.getChipRevision(), ESP.getChipCores());
  Serial.printf("Flash: %u MB\n", ESP.getFlashChipSize() / (1024 * 1024));
  Serial.printf("esp32 board package: %d.%d.%d\n",
                ESP_ARDUINO_VERSION_MAJOR, ESP_ARDUINO_VERSION_MINOR, ESP_ARDUINO_VERSION_PATCH);
}

void loop() {
  // Non-blocking timing with millis() instead of delay().
  // The whole game will be built on this pattern, so it starts here.
  if (millis() - lastToggle >= BLINK_MS) {
    lastToggle = millis();
    ledOn = !ledOn;
    digitalWrite(LED_PIN, ledOn ? HIGH : LOW);
    Serial.println(ledOn ? "LED on" : "LED off");
  }
}
