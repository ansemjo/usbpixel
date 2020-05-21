#include <NeoPixelBus.h>

// number of pixels in strip
#define PIXELS 20

// if used with DotStarMethod (bitbang)
#define PIN_CLK  2
#define PIN_DATA 14 // Uno D14 == A0

// override data pin for usbpixel
#ifdef USBPIXEL
#undef PIN_DATA
#define PIN_DATA 4
#endif

// global luminance value
#define L 0.1

// for software bit bang
//NeoPixelBus<DotStarBgrFeature, DotStarMethod> strip(PIXELS, PIN_CLK, PIN_DATA);
// for hardware SPI
//NeoPixelBus<DotStarBgrFeature, DotStarSpiMethod> strip(PIXELS);
// neopixels with onewire interface
NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod> strip(PIXELS, PIN_DATA);

// define a few useful solid colors
HslColor WHITE  (         0, 0, L);
HslColor OFF    (         0, 0, 0);
HslColor RED    (   0.0/360, 1, L);
HslColor YELLOW (  60.0/360, 1, L);
HslColor GREEN  ( 120.0/360, 1, L);
HslColor CYAN   ( 180.0/360, 1, L);
HslColor BLUE   ( 240.0/360, 1, L);
HslColor PINK   ( 300.0/360, 1, L);

void setup() {

  // enable second pixel ground for usbpixel
#ifdef USBPIXEL
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);
#endif
  
  // reset strip to off state
  strip.Begin();
  strip.ClearTo(OFF);
  strip.Show();

}

// fill the strip with a rainbow section
void rainbowfill(float hue, float angle) {
  float increment = angle / PIXELS;
  for (int i = 0; i < PIXELS; i++) {
    strip.SetPixelColor(i, HslColor( hue + increment*i, 1, L ));
  }
  strip.Show();
}

// fading rainbow animation
void rainbowfade(float angle, float add) {
  static float hue = 0.0;
  rainbowfill(hue, angle);
  hue += add;
  if (hue >= 1.0) {
    hue -= 1;
  }
}

// blink the strip in individual solid colors
void blink_rgbw(int wait) {
  for (int i = 0; i < PIXELS; i++) {
    switch (i % 8) {
      case 0: strip.SetPixelColor(i, RED);    break;
      case 1: strip.SetPixelColor(i, GREEN);  break;
      case 2: strip.SetPixelColor(i, BLUE);   break;
      case 3: strip.SetPixelColor(i, WHITE);  break;
      case 4: strip.SetPixelColor(i, YELLOW); break;
      case 5: strip.SetPixelColor(i, CYAN);   break;
      case 6: strip.SetPixelColor(i, PINK);   break;
      case 7: strip.SetPixelColor(i, WHITE);  break;
    }
  }
  strip.Show();
  delay(wait);
  strip.ClearTo(OFF);
  strip.Show();
  delay(wait);
}

void loop() {
  rainbowfade( 120.0/360, 0.5/360);
  delay(10);
}
