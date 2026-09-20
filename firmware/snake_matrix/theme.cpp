// =====================================================================
// theme.cpp
// =====================================================================

#include "theme.h"
#include "display.h"

static uint32_t snakeRgb = DEFAULT_SNAKE_RGB;
static uint32_t foodRgb = DEFAULT_FOOD_RGB;

static bool sparkleActive = false;
static int sparkleX = 0;
static int sparkleY = 0;
static unsigned long sparkleStart = 0;


// ---------- Colours ----------

void themeSetSnakeColour(uint32_t rgb) { snakeRgb = rgb; }
void themeSetFoodColour(uint32_t rgb)  { foodRgb = rgb; }
uint32_t themeSnakeColour()            { return snakeRgb; }
uint32_t themeFoodColour()             { return foodRgb; }


// A colour picker lets anyone choose dark colours, like navy #000040.
// On the matrix, every colour is scaled down again by BRIGHTNESS, so a
// dark colour would barely show, or vanish. The fix: keep the colour's
// hue and mix, but raise it so its strongest channel is 255.
//   #336699 -> #55aaff   (same blue, full brightness)
// Brightness stays controlled in one place: BRIGHTNESS in config.h.
uint32_t themeNormalise(uint32_t rgb) {
  // Pull the three channels out of 0xRRGGBB. >> shifts bits right,
  // & 0xFF keeps only the lowest 8 bits (one channel).
  uint32_t r = (rgb >> 16) & 0xFF;
  uint32_t g = (rgb >> 8) & 0xFF;
  uint32_t b = rgb & 0xFF;

  uint32_t top = r;
  if (g > top) top = g;
  if (b > top) top = b;

  if (top == 0) {
    return 0xFFFFFF;          // black can't be shown on LEDs: use white
  }

  r = r * 255 / top;
  g = g * 255 / top;
  b = b * 255 / top;
  return (r << 16) | (g << 8) | b;   // pack back into 0xRRGGBB
}


// ---------- What to draw ----------

CRGB themeHead() {
  // beatsin8 gives a smooth wave between two values, timed from the
  // clock: here between 40 and 150, HEAD_PULSE_BPM times a minute.
  // blend() mixes the snake colour toward white by that amount (0-255).
  uint8_t whiteness = beatsin8(HEAD_PULSE_BPM, 40, 150);
  return blend(CRGB(snakeRgb), CRGB::White, whiteness);
}


CRGB themeBody(int i, int length) {
  // The neck (i = 1) is full strength; the tail tip drops to TAIL_MIN_LEVEL.
  // nscale8(level) scales the colour to level/255 of its brightness.
  uint8_t level = 255 - (uint32_t)(255 - TAIL_MIN_LEVEL) * i / length;
  CRGB colour = CRGB(snakeRgb);
  colour.nscale8(level);
  return colour;
}


CRGB themeFood() {
  // A slower, subtler pulse than the head, so food catches the eye
  // without looking like a second head.
  CRGB colour = CRGB(foodRgb);
  colour.nscale8(beatsin8(30, 170, 255));
  return colour;
}


// ---------- Eat sparkle ----------
// A small cross of food-coloured pixels bursts out from where the food
// was: one pixel away for the first half, two pixels away for the
// second, fading out the whole time.

void themeStartSparkle(int x, int y) {
  sparkleX = x;
  sparkleY = y;
  sparkleStart = millis();
  sparkleActive = true;
}


void themeDrawSparkle() {
  if (!sparkleActive) {
    return;
  }
  unsigned long age = millis() - sparkleStart;
  if (age >= SPARKLE_MS) {
    sparkleActive = false;
    return;
  }

  uint8_t level = 255 - age * 255 / SPARKLE_MS;      // fades from 255 to 0
  int reach = (age < SPARKLE_MS / 2) ? 1 : 2;        // spreads outward

  CRGB colour = CRGB(foodRgb);
  colour.nscale8(level);

  // Off-grid pixels are ignored by displaySetPixel, so sparkles near
  // the edge are simply cut off.
  displaySetPixel(sparkleX + reach, sparkleY, colour);
  displaySetPixel(sparkleX - reach, sparkleY, colour);
  displaySetPixel(sparkleX, sparkleY + reach, colour);
  displaySetPixel(sparkleX, sparkleY - reach, colour);
}
