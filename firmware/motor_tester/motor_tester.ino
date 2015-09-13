/*
 * motor_tester.ino
 *
 * Test harness setup to measure motor performance parameters with the H-bridge
 * I'm using (VNH5019). Specifically, I'm measuring RPM using an IR tachyometer
 * and voltage/current with a pair of multimeters.
 *
 * For control, I'm using a rotary encoder with an RGB LED built in (which
 * won't be used for much, but wanted to test it and have some blinkenlights).
 * The built-in button will handle stop/start of motor, while the encoder will
 * set speed (via PWM) and direction (CW or CCW cranking).
 *
 */

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>
#include <string.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

////////////////////////////////////////////////////////////////////////
// PIN ASSIGNMENT
////////////////////////////////////////////////////////////////////////

#define MOTOR_ON   3
#define MOTOR_IN_A 5
#define MOTOR_PWM  6
#define MOTOR_IN_B 7
#define MOTOR_CS   A0

#define SPD_KNOB0  13
#define SPD_KNOB1  9
#define SPD_KNOB2  8
#define SPD_KNOB3  10
#define SPD_KNOB4  11
#define SPD_KNOB5  12

#define SPD_LEVEL_0 0
#define SPD_LEVEL_1 51
#define SPD_LEVEL_2 102
#define SPD_LEVEL_3 153
#define SPD_LEVEL_4 204
#define SPD_LEVEL_5 255

#define SPD_MASK_0 (1<<0)
#define SPD_MASK_1 (1<<1)
#define SPD_MASK_2 (1<<2)
#define SPD_MASK_3 (1<<3)
#define SPD_MASK_4 (1<<4)
#define SPD_MASK_5 (1<<5)

////////////////////////////////////////////////////////////////////////
// OTHER CONSTANTS
////////////////////////////////////////////////////////////////////////

// Hardware serial speed for debuggin'
#define BAUD_RATE 9600

// Display params
#define DISPLAY_ADDR     SSD1306_I2C_ADDRESS
#define DISPLAY_MODE     SSD1306_SWITCHCAPVCC
#define DISPLAY_TEXTSIZE 2
#define DISPLAY_COLOR    WHITE
#define DISPLAY_RST_PIN  4

////////////////////////////////////////////////////////////////////////
// GLOBAL STATE VARIABLES
////////////////////////////////////////////////////////////////////////

// Display controller object
Adafruit_SSD1306 display(DISPLAY_RST_PIN);

// Holds the current speed state
uint8_t speed = 0;

// Motor on/off state via switch
boolean motor_on = false;

// Output of the current sense pin
float current = 0;

uint8_t last_value = 0;

////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
////////////////////////////////////////////////////////////////////////

void printStatus() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("PWM ");
  if (motor_on) {
    display.println(speed, DEC);
  } else {
    display.println("off");
  }
  display.print("CS  ");
  display.println(current, DEC);
  display.display();
}

uint8_t getSpeed() {
  uint8_t value = 0;
  uint8_t retval = 0;

  value |= (digitalRead(SPD_KNOB0) == LOW ? 1 : 0) << 0;
  value |= (digitalRead(SPD_KNOB1) == LOW ? 1 : 0) << 1;
  value |= (digitalRead(SPD_KNOB2) == LOW ? 1 : 0) << 2;
  value |= (digitalRead(SPD_KNOB3) == LOW ? 1 : 0) << 3;
  value |= (digitalRead(SPD_KNOB4) == LOW ? 1 : 0) << 4;
  value |= (digitalRead(SPD_KNOB5) == LOW ? 1 : 0) << 5;

  if (value & SPD_MASK_1) {
    retval = SPD_LEVEL_1;
  } else if (value & SPD_MASK_2) {
    retval = SPD_LEVEL_2;
  } else if (value & SPD_MASK_3) {
    retval = SPD_LEVEL_3;
  } else if (value & SPD_MASK_4) {
    retval = SPD_LEVEL_4;
  } else if (value & SPD_MASK_5) {
    retval = SPD_LEVEL_5;
  } else {
    retval = speed;
  }

  if (value != last_value) {
    last_value = value;
    Serial.println(value, BIN);
  }

  return retval;

}

float getCurrentSense() {
  int reading = analogRead(MOTOR_CS);
  return (reading / 1024.0) * 5.0 / 0.14;
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
  display.println("HAI");
  display.display();

  // Configure inputs
  pinMode(MOTOR_ON, INPUT_PULLUP);
  pinMode(MOTOR_CS, INPUT);
  pinMode(SPD_KNOB0, INPUT_PULLUP);
  pinMode(SPD_KNOB1, INPUT_PULLUP);
  pinMode(SPD_KNOB2, INPUT_PULLUP);
  pinMode(SPD_KNOB3, INPUT_PULLUP);
  pinMode(SPD_KNOB4, INPUT_PULLUP);
  pinMode(SPD_KNOB5, INPUT_PULLUP);

  // Configure motor controller
  pinMode(MOTOR_IN_A, OUTPUT);
  pinMode(MOTOR_PWM, OUTPUT);
  pinMode(MOTOR_IN_B, OUTPUT);
  digitalWrite(MOTOR_IN_A, LOW);
  digitalWrite(MOTOR_IN_B, LOW);
  analogWrite(MOTOR_PWM, 0);

  Serial.begin(BAUD_RATE);
  Serial.println("HAI");

}

////////////////////////////////////////////////////////////////////////
// MAIN LOOP
////////////////////////////////////////////////////////////////////////

void loop() {

  motor_on = digitalRead(MOTOR_ON) ? LOW : HIGH;
  speed = getSpeed();
  current = getCurrentSense();

  if (!motor_on) {
    digitalWrite(MOTOR_IN_A, LOW);
    digitalWrite(MOTOR_IN_B, LOW);
    analogWrite(MOTOR_PWM, 0);
  } else {
    digitalWrite(MOTOR_IN_A, HIGH);
    digitalWrite(MOTOR_IN_B, LOW);
    analogWrite(MOTOR_PWM, speed);
  }

  printStatus();
  delay(100);
}

// vi: syntax=arduino
