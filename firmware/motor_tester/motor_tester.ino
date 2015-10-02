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

#define MOTOR_IN_A 9
#define MOTOR_PWM  10
#define MOTOR_IN_B 11
#define MOTOR_ON   12
#define MOTOR_CS   A0

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
#define DISPLAY_RST_PIN  8

////////////////////////////////////////////////////////////////////////
// GLOBAL STATE VARIABLES
////////////////////////////////////////////////////////////////////////

// Display controller object
Adafruit_SSD1306 display(DISPLAY_RST_PIN);

// Holds the current speed state
uint8_t speed = 0;

// Motor on/off state via switch
boolean motor_on = false;

// Output of the current sense pin (in mA)
float current = 0;

uint8_t last_value = 0;

////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
////////////////////////////////////////////////////////////////////////

void printStatus() {
  display.clearDisplay();
  display.setCursor(0, 0);
  if (motor_on) {
    display.print("ON  ");
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    display.print("OFF ");
    digitalWrite(LED_BUILTIN, LOW);
  }
  display.println(speed, DEC);
  display.print("CS  ");
  display.print(current, 1);
  display.println("mA");
  display.display();
}

uint8_t getSpeed() {
  uint8_t value = 0;
  uint8_t retval = 0;

  value |= (digitalRead(0) == LOW ? 1 : 0) << 0;
  value |= (digitalRead(1) == LOW ? 1 : 0) << 1;
  value |= (digitalRead(2) == LOW ? 1 : 0) << 2;
  value |= (digitalRead(3) == LOW ? 1 : 0) << 3;
  value |= (digitalRead(4) == LOW ? 1 : 0) << 4;
  value |= (digitalRead(5) == LOW ? 1 : 0) << 5;
  value |= (digitalRead(6) == LOW ? 1 : 0) << 6;
  value |= (digitalRead(7) == LOW ? 1 : 0) << 7;
  retval = value;

  return retval;

}

float getCurrentSense() {
  int reading = analogRead(MOTOR_CS);
  return (reading / 1024.0) * 5.0 / 0.14 * 1000.0;
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

  // Configure inputs
  pinMode(MOTOR_ON, INPUT_PULLUP);
  pinMode(MOTOR_CS, INPUT);
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  // Configure motor controller
  pinMode(MOTOR_IN_A, OUTPUT);
  pinMode(MOTOR_PWM, OUTPUT);
  pinMode(MOTOR_IN_B, OUTPUT);
  digitalWrite(MOTOR_IN_A, LOW);
  digitalWrite(MOTOR_IN_B, LOW);
  analogWrite(MOTOR_PWM, 0);

  //Serial.begin(BAUD_RATE);
  //Serial.println("HAI");

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
