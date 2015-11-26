/*
 * display.cpp
 *
 * Control & update SSD1306 OLED display.
 *
 * Author: Sean "schizobovine" Caulfield <sean@yak.net>
 * License: GPLv2 (firmware) / CC4.0-BY-SA (documentation, hardware)
 *
 */

/*
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/power.h>
#include <avr/sleep.h>

#include <EnableInterrupt.h>


#define SERIAL_DEBUG 1
#define SERIAL_BAUD_RATE 9600

#if SERIAL_DEBUG
#define DEBUG_PRINTLN(msg) Serial.println((msg))
#define DEBUG_PRINT(msg) Serial.print((msg))
#else
#define DEBUG_PRINTLN(msg)
#define DEBUG_PRINT(msg)
#endif
*/

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Bounce2.h>

#include "vnh5019.h"
#include "firemode.h"
#include "clip.h"
#include "tach.h"
#include "rapidshark_mk_iv.h"

////////////////////////////////////////////////////////////////////////
// GLOBAL STATE VARIABLES
////////////////////////////////////////////////////////////////////////

// NB These are extern since they are actually allocated in the main .ino file,
// so we only have to declare the name here.

// Display controller
extern Adafruit_SSD1306 display;

// Motor controllers
extern VNH5019 motor_accel;
extern VNH5019 motor_push;

// Button/switch debouncers
extern Bounce switchPusher;
extern Bounce switchClipDetect;
extern Bounce switchFireTrigger;
extern Bounce switchAccelTrigger;

// Current & total ammo counters
extern AmmoClip ammo_clip;

// Current fire control mode
extern FireMode fireMode;

// Monitor flywheel rotational velocity
extern Tachometer tach;

////////////////////////////////////////////////////////////////////////
// DISPLAY FUNCTIONS
////////////////////////////////////////////////////////////////////////

/*
 * displayRefresh
 *
 * Update status information on the display.
 *
 */
void displayRefresh() {

  display.clearDisplay();

  displayTextNormal();
  display.setTextSize(4);
  display.setCursor(40, 0);
  display.print(ammo_clip.getCurrent(), DEC);

  display.setTextSize(1);

  displayLabel( 0, 40, "ACC" , (IS_ACC_TRIG_CLOSED));
  displayLabel( 0, 48, "FIRE", (IS_FIRE_TRIG_CLOSED));
  displayLabel( 0, 56, "PUSH", (IS_PUSHER_EXTENDED));
  displayLabel(30, 40, "CLIP", (IS_CLIP_INSERTED));

  display.setCursor(30, 48);
  switch (fireMode.getMode()) {
    case MODE_SEMI_AUTO:
      display.print("SEMI");
      break;
    case MODE_BURST:
      display.print("BURST");
      break;
    case MODE_FULL_AUTO:
      display.print("AUTO");
      break;
    default:
      display.print("???");
      break;
  }

  displayLabel(72, 40, "FLY", (motor_accel.isGoing()));
  displayLabel(66, 48, "PUSH", (motor_push.isGoing()));
  displayLabel(72, 56, "RPM", false);

  display.setCursor(96, 40);
  display.print(motor_accel.getSpeed(), DEC);

  display.setCursor(96, 48);
  display.print(motor_push.getSpeed(), DEC);

  display.setCursor(96, 56);
  display.print(tach.rpm(), DEC);

  display.display();

}

void displayLabel(uint8_t x, uint8_t y, const char *text, bool invert) {
  display.setCursor(x, y);
  if (invert) {
    displayTextFlipped();
  } else {
    displayTextNormal();
  }
  display.print(text);
  displayTextNormal();
}

void displayTextNormal() {
  display.setTextColor(WHITE);
}

void displayTextFlipped() {
  display.setTextColor(BLACK, WHITE);
}

/*
 * init_display - Boot up display and print out something to show it works
 */
void displayInit() {
  Wire.begin();
  display.begin(DISP_MODE, DISP_ADDR);
  display.clearDisplay();
  display.setTextColor(DISP_COLOR);
  display.setTextSize(DISP_TEXT_LARGE);
  display.println();
  display.print("RapidShark");
  display.print("  Mark IV");
  display.dim(false);
  display.setTextWrap(false);
  display.display();
}
