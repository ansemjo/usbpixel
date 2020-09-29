package main

import (
	"flag"
	"fmt"
	"log"
	"time"

	"github.com/google/gousb"
)

const LED_OFF = 0
const LED_ON = 1
const LED_HUE = 2
const LED_LUMINANCE = 3

const CTL_RETRIES = 3

func control(dev *gousb.Device, ctl uint8, val, idx uint16, buf []byte) (n int, err error) {
	for i := 0; i < CTL_RETRIES; i++ {
		n, err = dev.Control(gousb.ControlVendor|gousb.ControlDevice, ctl, val, idx, buf)
		if err != gousb.ErrorIO {
			return
		}
		log.Printf("I/O error, attempt %d/%d", i+1, CTL_RETRIES)
	}
	return
}

var turnon = flag.Bool("on", false, "turn the pixels on")
var turnoff = flag.Bool("off", false, "turn the pixels off")
var sethue = flag.Bool("hue", false, "set to specific rgbw color")
var hue_red = flag.Int("r", 0, "hue: red")
var hue_green = flag.Int("g", 0, "hue: green")
var hue_blue = flag.Int("b", 0, "hue: blue")
var hue_white = flag.Int("w", 0, "hue: white")
var luminance = flag.Float64("l", -1, "set luminance")

func main() {

	flag.Parse()

	ctx := gousb.NewContext()
	defer ctx.Close()

	dev, err := ctx.OpenDeviceWithVIDPID(0x16C0, 0x05DC)
	if err != nil {
		log.Fatal(err)
	}
	defer dev.Close()
	dev.ControlTimeout = 200 * time.Millisecond

	if *turnon {
		_, err = control(dev, LED_ON, 0, 0, nil)
	} else if *turnoff {
		_, err = control(dev, LED_OFF, 0, 0, nil)
	} else if *sethue {
		_, err = control(dev, LED_HUE, uint16(*hue_red|(*hue_green<<8)), uint16(*hue_blue|(*hue_white<<8)), nil)
	} else if *luminance != -1 {
		lum := uint16(*luminance * 0xffff)
		log.Printf("set luminance: %f * 0xffff = %d", *luminance, lum)
		_, err = control(dev, LED_LUMINANCE, lum, 0, nil)
	} else {
		err = fmt.Errorf("no action given")
	}
	if err != nil {
		log.Fatal(err)
	}

	// buf := make([]byte, 32)
	// // n, err := dev.Control(gousb.ControlVendor|gousb.ControlDevice|gousb.ControlIn, LED_HELLO, 0, 0, buf)
	// n, err := control(dev, LED_HELLO, buf)
	// if err != nil {
	// 	log.Fatal(err)
	// }
	// if string(buf[:n]) != "Hello, World!\x00" {
	// 	log.Fatalf("unexpected string: %#v", string(buf[:n]))
	// }
	// log.Printf("Read data: %s\n", string(buf[:n]))

}
