/*
 * battery_tester.ino
 *
 * Test harness setup to see if my battery detector idea will work.
 *
 */

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>

////////////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////////////

// Which battery cell is hooked up where
#define CELL3 A3
#define CELL2 A2
#define CELL1 A1

// Hardware serial speed for debuggin'
#define BAUD_RATE 9600

// Display params
#define DISPLAY_ADDR     SSD1306_I2C_ADDRESS
#define DISPLAY_MODE     SSD1306_SWITCHCAPVCC
#define DISPLAY_TEXTSIZE 2
#define DISPLAY_COLOR    WHITE
#define DISPLAY_RST_PIN  8

////////////////////////////////////////////////////////////////////////
// GLOBAL STATE VARIABLES
////////////////////////////////////////////////////////////////////////

// Display controller object
Adafruit_SSD1306 display(DISPLAY_RST_PIN);

// Current values for voltage
float cell1v = 0.0;
float cell2v = 0.0;
float cell3v = 0.0;

////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
////////////////////////////////////////////////////////////////////////

void refresh_display() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(cell1v, DEC);
  display.setCursor(42, 0);
  display.print(cell2v, DEC);
  display.setCursor(84, 0);
  display.print(cell3v, DEC);
  display.display();
}

////////////////////////////////////////////////////////////////////////
// STARTUP CODE
////////////////////////////////////////////////////////////////////////

void setup() {

  // Boot display
  Wire.begin();
  display.begin(DISPLAY_MODE, DISPLAY_ADDR);
  display.clearDisplay();
  display.setTextColor(DISPLAY_COLOR);
  display.setTextSize(DISPLAY_TEXTSIZE);
  display.setTextWrap(false);
  display.println("HAI");
  display.display();

  // Setup serial debugging
  Serial.begin(BAUD_RATE);
  Serial.println("HAI");

  // Set sleep mode for later use
  set_sleep_mode(SLEEP_MODE_IDLE);

}

////////////////////////////////////////////////////////////////////////
// MAIN LOOP
////////////////////////////////////////////////////////////////////////

void loop() {

  // Display stuff
  refresh_display();

  // Go to sleep until interrupted
  //sleep_mode();
  delay(1000);

}

// vi: syntax=arduino
