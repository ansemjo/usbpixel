#include <NeoPixelBus.h>
#include <usbdrv.h>

// ---------- neopixel control ---------

// number of pixels in strip
#define PIXELS 2

// global luminance value
#define L 0.01

// neopixels with onewire interface
#define PIN_DATA 4
NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod> strip(PIXELS, PIN_DATA);

// define a few useful solid colors
HsbColor WHITE  (         0, 0, L);
HsbColor OFF    (         0, 0, 0);
HsbColor RED    (   0.0/360, 1, L);
HsbColor YELLOW (  60.0/360, 1, L);
HsbColor GREEN  ( 120.0/360, 1, L);
HsbColor CYAN   ( 180.0/360, 1, L);
HsbColor BLUE   ( 240.0/360, 1, L);
HsbColor PINK   ( 300.0/360, 1, L);

// fill the strip with a rainbow section
void rainbowfill(float hue, float angle) {
  float increment = angle / PIXELS;
  for (int i = 0; i < PIXELS; i++) {
    strip.SetPixelColor(i, HsbColor( hue + increment*i, 1, L ));
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

volatile void updateSolid(HsbColor color) {
  for (unsigned i = 0; i < PIXELS; i++) {
    strip.SetPixelColor(i, color);
  }
}

// ---------- v-usb interface ---------

// a couple of control messages
#define USB_LED_OFF     0
#define USB_LED_ON      1
#define USB_HELLOWORLD  2
#define CTL_HUE         3

static uchar helloWorld[14] = "Hello, World!";

// react to custom control messages
usbMsgLen_t usbFunctionSetup(uchar data[8]) {
  usbRequest_t *rq = (void *)data;
  static uchar dataBuffer[4];

  switch (rq->bRequest) {
    
    case USB_LED_OFF:
      updateSolid(BLUE);
      return 0;
    
    case USB_LED_ON:
      updateSolid(RED);
      return 0;
    
    case USB_HELLOWORLD:
      usbMsgPtr = helloWorld;
      return sizeof(helloWorld);

    case CTL_HUE:
      RgbwColor c = RgbwColor(rq->wValue.bytes[0], rq->wValue.bytes[1], rq->wIndex.bytes[0], rq->wIndex.bytes[1]);
      for (unsigned i = 0; i < PIXELS; i++) {
        strip.SetPixelColor(i, c);
      }
      return 0;

  }
  return 0;

}

// ---------- microcontroller init ---------

void setup() {

  // enable second pixel ground
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);
  
  // reset strip to off state
  strip.Begin();
  strip.ClearTo(GREEN);
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
  strip.Show();
    
  //rainbowfade( 120.0/360, 0.5/360);
  //delay(10);
}
