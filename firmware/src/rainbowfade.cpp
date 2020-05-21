#include <NeoPixelBus.h>
#include <usbdrv.h>


// ---------- neopixel control ---------

// number of pixels in strip
#define PIXELS 20

// global luminance value
#define L 0.01

// neopixels with onewire interface
#define PIN_DATA 4
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

// ---------- v-usb interface ---------

USB_PUBLIC uchar usbFunctionSetup(uchar data[8]) {
	return 0; // do nothing for now
}

// ---------- microcontroller init ---------

void setup() {

  // enable second pixel ground
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);
  
  // reset strip to off state
  strip.Begin();
  strip.ClearTo(OFF);
  strip.Show();

  // initialize usb
  usbInit();
  usbDeviceDisconnect();
  delay(500);
  usbDeviceConnect();
  sei();

}

void loop() {
  usbPoll();
  //rainbowfade( 120.0/360, 0.5/360);
  //delay(10);
}
