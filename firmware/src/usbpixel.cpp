#include <NeoPixelBus.h>
#include <usbdrv.h>

// ---------- neopixel control ---------

// number of pixels in strip
#define PIXELS 2

// global luminance value
volatile float luminance = 0.1;

enum pixelState {
  OFF,
  SOLID,
  RAINBOW,
};
static enum pixelState state = RAINBOW;

// neopixels with onewire interface
#define PIN_DATA 4
NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod> strip(PIXELS, PIN_DATA);

// define a few useful solid colors
#define WHITE(L)  HsbColor (         0, 0, L)
#define OFF(L)    HsbColor (         0, 0, 0)
#define RED(L)    HsbColor (   0.0/360, 1, L)
#define YELLOW(L) HsbColor (  60.0/360, 1, L)
#define GREEN(L)  HsbColor ( 120.0/360, 1, L)
#define CYAN(L)   HsbColor ( 180.0/360, 1, L)
#define BLUE(L)   HsbColor ( 240.0/360, 1, L)
#define PINK(L)   HsbColor ( 300.0/360, 1, L)

// fill the strip with a rainbow section
void rainbowfill(float hue, float angle) {
  float increment = angle / PIXELS;
  for (int i = 0; i < PIXELS; i++) {
    strip.SetPixelColor(i, HsbColor( hue + increment*i, 1, luminance ));
  }
}

// fading rainbow animation
void rainbowfade(float angle, float add) {
  static float hue = random(1024) / 1024.0;
  rainbowfill(hue, angle);
  hue += add;
  if (hue >= 1.0) {
    hue -= 1;
  }
}

volatile void updateSolid(HsbColor color) {
  for (unsigned i = 0; i < PIXELS; i++) {
    strip.SetPixelColor(i, color);
  }
}

// ---------- v-usb interface ---------

// control message types
enum usbCtlType {
  LED_OFF = 0,
  LED_ON,
  SET_HUE,
  SET_LUMINANCE
};

// react to custom control messages
usbMsgLen_t usbFunctionSetup(uchar data[8]) {
  usbRequest_t *rq = (void *)data;

  switch ((usbCtlType)rq->bRequest) {
    
    case LED_OFF: {
      state = OFF;
      strip.ClearTo(OFF(0));
      strip.Show();
      return 0;
    }

    case LED_ON: {
      state = RAINBOW;
      return 0;
    }

    case SET_HUE: {
      state = SOLID;
      RgbwColor c = RgbwColor(rq->wValue.bytes[0], rq->wValue.bytes[1], rq->wIndex.bytes[0], rq->wIndex.bytes[1]);
      for (int i = 0; i < PIXELS; i++) {
        strip.SetPixelColor(i, c);
      }
      return 0;
    }

    case SET_LUMINANCE: {
      float l = rq->wValue.word;
      luminance = (l / 0xffff);
      return 0;
    }

  }
  return 0;

}

// ---------- microcontroller init ---------

void setup() {

  // random seed
  // THIS IS NOT CRYPTOGRAPHICALLY SECURE
  randomSeed(analogRead(1));

  // enable second pixel ground
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);

  // reset strip to off state
  strip.Begin();
  strip.ClearTo(OFF(0));
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

  // rainbowfader
  static unsigned long last = 0;
  if (millis() - last > 10) {
    if (state == RAINBOW)
      rainbowfade(0.1, 0.5/360);
    last = millis();
  }
  if (state != OFF)
    strip.Show();
    
}
