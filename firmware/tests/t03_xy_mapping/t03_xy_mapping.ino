// =====================================================================
// t03_xy_mapping
// ---------------------------------------------------------------------
// Teaches the code to think in (x, y) instead of LED numbers.
//
// The matrix is really one long strip of 256 LEDs folded into columns.
// Phase 2 showed us how it's folded:
//   - LED 0 is top left
//   - column 0 runs top to bottom (LEDs 0-7)
//   - column 1 runs bottom to top (LEDs 8-15)
//   - column 2 runs top to bottom again, and so on
//
// XY(x, y) turns a grid position into the LED number on the strip.
//   x = column, 0 (left) to 31 (right)
//   y = row,    0 (top)  to 7  (bottom)
//
// Hold the matrix the same way as in phase 2 (plug on the left).
//
// The demo cycles through five patterns, each checking a different part
// of the mapping. You can also type two numbers in Serial Monitor, like
// "5 3", to light a single pixel. Type "demo" to restart the demo.
//
// Serial Monitor: 115200 baud, line ending set to "New Line".
// =====================================================================

#include <FastLED.h>


// ---------- Settings ----------
const int DATA_PIN = 2;
const int WIDTH = 32;                 // columns
const int HEIGHT = 8;                 // rows
const int NUM_LEDS = WIDTH * HEIGHT;  // 256
const uint8_t BRIGHTNESS = 40;
const uint32_t MAX_MILLIAMPS = 300;

const unsigned long HOLD_MS = 3000;       // how long each still pattern shows
const unsigned long COLUMN_STEP_MS = 80;  // column sweep speed
const unsigned long ROW_STEP_MS = 250;    // row sweep speed

CRGB leds[NUM_LEDS];


// ---------- Demo state ----------
// Stages: 0 corners, 1 border, 2 diagonals, 3 column sweep, 4 row sweep
const int NUM_STAGES = 5;
int stage = 0;
unsigned long stageStart = 0;
unsigned long lastStep = 0;
int sweepPos = 0;           // which column or row the sweep has reached
bool demoRunning = true;    // false while you're testing pixels by hand


// ---------- Serial input ----------
String inputLine = "";      // characters typed so far, until Enter


// ---------------------------------------------------------------------
// XY: the heart of this phase.
// Returns the LED number for column x, row y, or -1 if (x, y) is off
// the grid. Returning -1 lets other code safely ignore off-grid
// positions, which the game will rely on (a snake head past the edge).
// ---------------------------------------------------------------------
int XY(int x, int y) {
  if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
    return -1;
  }

  // Every column before this one holds HEIGHT LEDs,
  // so column x starts at LED number x * HEIGHT.
  int columnStart = x * HEIGHT;

  if (x % 2 == 0) {
    // Even columns (0, 2, 4...) run top to bottom:
    // row y is simply y LEDs into the column.
    return columnStart + y;
  } else {
    // Odd columns (1, 3, 5...) run bottom to top:
    // row 0 is the LAST LED of the column, row 7 the first.
    return columnStart + (HEIGHT - 1 - y);
  }
}


// Colours one pixel by grid position. Off-grid positions are ignored,
// so drawing code never has to worry about going out of bounds.
void setPixel(int x, int y, CRGB colour) {
  int i = XY(x, y);
  if (i >= 0) {
    leds[i] = colour;
  }
}


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("t03_xy_mapping");
  Serial.println("Type two numbers like '5 3' to light one pixel, or 'demo' to restart the demo.");

  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_MILLIAMPS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear(true);

  enterStage(0);
}


void loop() {
  readSerial();               // always listen for typed commands
  if (!demoRunning) {
    return;                   // demo paused while you test pixels by hand
  }

  unsigned long now = millis();

  if (stage <= 2) {
    // Still patterns: show for HOLD_MS, then move on.
    if (now - stageStart >= HOLD_MS) {
      nextStage();
    }

  } else if (stage == 3) {
    // Column sweep: one vertical line, moving left to right.
    if (now - lastStep >= COLUMN_STEP_MS) {
      lastStep = now;
      if (sweepPos >= WIDTH) {      // passed the last column: done
        nextStage();
        return;
      }
      FastLED.clear();
      for (int y = 0; y < HEIGHT; y++) {
        setPixel(sweepPos, y, CRGB::Cyan);
      }
      FastLED.show();
      sweepPos++;
    }

  } else if (stage == 4) {
    // Row sweep: one horizontal line, moving top to bottom.
    if (now - lastStep >= ROW_STEP_MS) {
      lastStep = now;
      if (sweepPos >= HEIGHT) {     // passed the last row: done
        nextStage();
        return;
      }
      FastLED.clear();
      for (int x = 0; x < WIDTH; x++) {
        setPixel(x, sweepPos, CRGB::Magenta);
      }
      FastLED.show();
      sweepPos++;
    }
  }
}


// Moves to the next stage, wrapping from the last back to the first.
// % (modulo) gives the remainder: (4 + 1) % 5 = 0.
void nextStage() {
  enterStage((stage + 1) % NUM_STAGES);
}


// Sets up a stage: draws the still patterns, or resets the sweeps.
void enterStage(int s) {
  stage = s;
  stageStart = millis();
  sweepPos = 0;
  lastStep = 0;               // lets a sweep draw its first step right away
  FastLED.clear();

  if (s == 0) {
    // Each corner in its own colour, so a flipped or rotated
    // mapping is obvious at a glance.
    setPixel(0, 0, CRGB::Red);
    setPixel(WIDTH - 1, 0, CRGB::Green);
    setPixel(0, HEIGHT - 1, CRGB::Blue);
    setPixel(WIDTH - 1, HEIGHT - 1, CRGB::Yellow);
    Serial.println("Corners: red top-left, green top-right, blue bottom-left, yellow bottom-right");

  } else if (s == 1) {
    // Top and bottom rows, then left and right columns.
    for (int x = 0; x < WIDTH; x++) {
      setPixel(x, 0, CRGB::White);
      setPixel(x, HEIGHT - 1, CRGB::White);
    }
    for (int y = 0; y < HEIGHT; y++) {
      setPixel(0, y, CRGB::White);
      setPixel(WIDTH - 1, y, CRGB::White);
    }
    Serial.println("Border: an unbroken frame around the edge");

  } else if (s == 2) {
    // x % HEIGHT counts 0-7 and repeats, so y climbs down one row
    // per column and restarts every 8 columns: four diagonals.
    for (int x = 0; x < WIDTH; x++) {
      setPixel(x, x % HEIGHT, CRGB::Orange);
    }
    Serial.println("Diagonals: four clean slopes, each going down to the right");

  } else if (s == 3) {
    Serial.println("Column sweep: a vertical line moving left to right");

  } else if (s == 4) {
    Serial.println("Row sweep: a horizontal line moving top to bottom");
  }

  FastLED.show();
}


// Collects typed characters. When Enter arrives, handles the whole line.
// Reading one character at a time never makes loop() wait.
void readSerial() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (inputLine.length() > 0) {
        handleCommand(inputLine);
      }
      inputLine = "";
    } else {
      inputLine += c;
    }
  }
}


// Understands two kinds of command: "demo", or two numbers "x y".
void handleCommand(String command) {
  command.trim();                 // strip stray spaces

  if (command == "demo") {
    demoRunning = true;
    Serial.println("Demo restarted");
    enterStage(0);
    return;
  }

  int x, y;
  // sscanf reads two whole numbers out of the text. It returns how
  // many it found, so 2 means the command looked like "5 3".
  if (sscanf(command.c_str(), "%d %d", &x, &y) == 2) {
    int i = XY(x, y);
    if (i < 0) {
      Serial.printf("(%d, %d) is off the grid. x is 0-31, y is 0-7\n", x, y);
      return;
    }
    demoRunning = false;          // pause the demo so the pixel stays lit
    FastLED.clear();
    leds[i] = CRGB::White;
    FastLED.show();
    Serial.printf("(%d, %d) -> LED %d\n", x, y, i);
  } else {
    Serial.println("Type two numbers like: 5 3   (or 'demo' to restart the demo)");
  }
}
