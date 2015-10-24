/*
 * rapidshark_mk_iv.ino
 *
 * Arduino microcontroller code to do All The Things to make darts fly.
 *
 * Author: Sean "schizobovine" Caulfield <sean@yak.net>
 * License: GPLv2 (firmware) / CC4.0-BY-SA (documentation, hardware)
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SERIAL_DEBUG 0

#if SERIAL_DEBUG
#define DEBUG_PRINTLN(msg) Serial.println((msg))
#define DEBUG_PRINT(msg) Serial.print((msg))
#else
#define DEBUG_PRINTLN(msg)
#define DEBUG_PRINT(msg)
#endif

////////////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////////////

//
// Pin assignments
//

#define PIN_DART_DETECT 2
#define PIN_SW_PUSH     3
#define PIN_SW_CLIP     4
#define PIN_SW_FIRE     5
#define PIN_SW_ACCEL    6
#define PIN_PUSH_A      7
#define PIN_ACCEL_A     8
#define PIN_ACCEL_PWM   9
#define PIN_ACCEL_B     10
#define PIN_PUSH_PWM    11
#define PIN_PUSH_B      12
#define PIN_BUTT_Z      A0
#define PIN_BUTT_Y      A1
#define PIN_BUTT_X      A2
#define PIN_DISP_RST    A3

//
// Display parameters
//

#define DISP_TEXTSIZE 1
#define DISP_COLOR    WHITE
#define DISP_ADDR     SSD1306_I2C_ADDRESS
#define DISP_MODE     SSD1306_SWITCHCAPVCC

////////////////////////////////////////////////////////////////////////
// GLOBAL STATE VARIABLES
////////////////////////////////////////////////////////////////////////

Adafruit_SSD1306 display(PIN_DISP_RST);

////////////////////////////////////////////////////////////////////////
// "HALPING" FUNCTIONS
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// STARTUP CODE
////////////////////////////////////////////////////////////////////////

void setup() {

#if SERIAL_DEBUG
  delay(200);
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println("HAI");
#endif

  // Set initial pin modes
  pinMode(PIN_DART_DETECT,  INPUT_PULLUP);
  pinMode(PIN_SW_PUSH,      INPUT_PULLUP);
  pinMode(PIN_SW_CLIP,      INPUT_PULLUP);
  pinMode(PIN_SW_FIRE,      INPUT_PULLUP);
  pinMode(PIN_SW_ACCEL,     INPUT_PULLUP);
  pinMode(PIN_ACCEL_A,      OUTPUT);
  pinMode(PIN_ACCEL_PWM,    OUTPUT);
  pinMode(PIN_ACCEL_B,      OUTPUT);
  pinMode(PIN_PUSH_A,       OUTPUT);
  pinMode(PIN_PUSH_PWM,     OUTPUT);
  pinMode(PIN_PUSH_B,       OUTPUT);
  pinMode(PIN_BUTT_Z,       INPUT_PULLUP);
  pinMode(PIN_BUTT_Y,       INPUT_PULLUP);
  pinMode(PIN_BUTT_X,       INPUT_PULLUP);

  // Initialize display
  Wire.begin();
  display.begin(DISP_MODE, DISP_ADDR);
  display.clearDisplay();
  display.setTextColor(DISP_COLOR);
  display.setTextSize(DISP_TEXTSIZE);
  display.println("I ARE BUTTS");
  display.display();

}

////////////////////////////////////////////////////////////////////////
// MAIN LOOP
////////////////////////////////////////////////////////////////////////

void loop() {

  delay(1000);

}
