/*
 * display.h
 *
 * Display code for the 128x64 OLED display.
 *
 * Author: Sean "schizobovine" Caulfield <sean@yak.net>
 * License: GPLv2 (firmware) / CC4.0-BY-SA (documentation, hardware)
 *
 */

#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/power.h>
#include <avr/sleep.h>

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


class Display : public Adafruit_SSD1306 {
  public:
    Display(int8_t rst_pin);
    void refreshDisplay(
        uint8_t ammoCount,
        bool accelTrigger,
        bool fireTrigger,
        bool pusherLimit,
        bool dartDetector,
        bool clipDetector,
    );

  display.clearDisplay();

  displayTextNormal();
  display.setTextSize(4);
  display.setCursor(40, 0);
  display.print("37");

  display.setTextSize(1);

  displayLabel( 0, 40, "ACC" , (switchAccelTrigger.read()));
  displayLabel( 0, 48, "FIRE", (switchFireTrigger.read()));
  displayLabel( 0, 56, "PUSH", (switchPusher.read()));
  displayLabel(30, 40, "DART", (dartDetector.read()));
  displayLabel(30, 48, "CLIP", (switchClipDetect.read()));

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
};

#endif
