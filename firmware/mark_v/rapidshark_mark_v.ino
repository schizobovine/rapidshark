/*
 * rapidshark_mark_v.ino
 *
 * Arduino microcontroller code to do All The Things to make darts fly.
 *
 * Author: Sean "schizobovine" Caulfield <sean@yak.net>
 * License: GPLv2 (firmware) / CC4.0-BY-SA (documentation, hardware)
 *
 */

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Bounce2.h>
#include <SPI.h>
#include <Wire.h>

////////////////////////////////////////////////////////////////////////
// PIN CONFIGURATION & COMPILE TIME SETTINGS
////////////////////////////////////////////////////////////////////////

#define PIN_TACHOMETER  2
#define PIN_SW_PUSH     3
#define PIN_SW_CLIP     4
#define PIN_SW_FIRE     6
#define PIN_SW_ACCEL    5
#define PIN_PUSH_A      7
#define 
#define 
#define 
#define PIN_PUSH_PWM    11
#define PIN_PUSH_B      12
#define PIN_BUTT_Z      A0
#define PIN_BUTT_Y      A1
#define PIN_BUTT_X      A2
#define PIN_DISP_RST    (-1)

#define DEBOUNCE_PUSH       (1)
#define DEBOUNCE_CLIP       (10)
#define DEBOUNCE_FIRE       (10)
#define DEBOUNCE_ACCEL      (10)

#define DISP_ADDR       0x3C
#define DISP_MODE       SSD1306_SWITCHCAPVCC
#define DISP_TEXT_SMALL 1
#define DISP_TEXT_LARGE 2
#define DISP_COLOR      WHITE


////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
////////////////////////////////////////////////////////////////////////

// Display controller
Adafruit_SSD1306 display(PIN_DISP_RST);

// Button/switch debouncers
Bounce switchPusher;
Bounce switchClipDetect;
Bounce switchFireTrigger;
Bounce switchAccelTrigger;

////////////////////////////////////////////////////////////////////////
// MOTOR STATE MACHINE
////////////////////////////////////////////////////////////////////////

/* TODO */

////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// SETUP
////////////////////////////////////////////////////////////////////////

void setup() {

  // Initialize debouncers & set pin mode
  switchPusher.attach(PIN_SW_PUSH, INPUT_PULLUP, DEBOUNCE_PUSH);
  switchClipDetect.attach(PIN_SW_CLIP, INPUT_PULLUP, DEBOUNCE_CLIP);
  switchFireTrigger.attach(PIN_SW_FIRE, INPUT_PULLUP, DEBOUNCE_FIRE);
  switchAccelTrigger.attach(PIN_SW_ACCEL, INPUT_PULLUP, DEBOUNCE_ACCEL);
  
  // Setup i2c & display
  Wire.begin();
  display.begin(DISP_MODE, DISP_ADDR);
  display.clearDisplay();
  display.setTextSize(DISP_TEXT_LARGE);
  display.setTextWrap(false);
  display.println();
  display.print("RapidShark");
  display.print("  Mark IV");
  display.dim(false);
  display.display();

}

////////////////////////////////////////////////////////////////////////
// MAIN LOOP
////////////////////////////////////////////////////////////////////////

void loop() {
}
