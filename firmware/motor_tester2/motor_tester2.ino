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
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include <string.h>
//#include <stdlib.h>
//#include <avr/io.h>
//#include <avr/pgmspace.h>

////////////////////////////////////////////////////////////////////////
// PIN ASSIGNMENT
////////////////////////////////////////////////////////////////////////

#define ENC_A      2
#define ENC_B      3
#define MOTOR_ON   4
#define MOTOR_PWM  5
#define DISP_RST   A3

////////////////////////////////////////////////////////////////////////
// GLOBAL STATE VARIABLES
////////////////////////////////////////////////////////////////////////

// Display
Adafruit_SSD1306 display(DISP_RST);

// Holds the current speed state
volatile uint8_t speed = 0;

// Motor on/off state via switch
boolean motor_on = false;

// Encoder variables
volatile uint8_t reading = 0;
boolean seen_a = false;
boolean seen_b = false;

////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
////////////////////////////////////////////////////////////////////////

void printStatus() {
  display.clearDisplay();
  display.setCursor(0, 0);
  if (motor_on) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
  display.println(speed, DEC);
  display.display();
}

////////////////////////////////////////////////////////////////////////
// INTERRUPT HANDLERS
////////////////////////////////////////////////////////////////////////

void intr_enc_a() {
  cli();
  reading = PIND & 0x0C;
  if ((reading == 0x0C) && seen_a) {
    if (speed > 0)
      speed--;
    seen_a = false;
    seen_b = false;
  } else if (reading == 0x04) {
    seen_b = true;
  }
  sei();
}

void intr_enc_b() {
  cli();
  reading = PIND & 0x0C;
  if ((reading == 0x0C) && seen_b) {
    if (speed < 0xFF)
      speed++;
    seen_a = false;
    seen_b = false;
  } else if (reading == 0x04) {
    seen_a = true;
  }
  sei();
}

////////////////////////////////////////////////////////////////////////
// STARTUP CODE
////////////////////////////////////////////////////////////////////////

void setup() {

  // Setup display
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setTextWrap(false);
  display.println("MOTR TESTR");
  display.display();

  // Configure rotary enccoder pins as inputs
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  pinMode(MOTOR_ON, INPUT_PULLUP);

  // Configure motor controller
  pinMode(MOTOR_PWM, OUTPUT);
  analogWrite(MOTOR_PWM, 0);

  // Attach interrupt handlers
  attachInterrupt(digitalPinToInterrupt(ENC_A), intr_enc_a, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_B), intr_enc_b, RISING);

}

////////////////////////////////////////////////////////////////////////
// MAIN LOOP
////////////////////////////////////////////////////////////////////////

void loop() {

  motor_on = digitalRead(MOTOR_ON) ? LOW : HIGH;

  if (!motor_on) {
    analogWrite(MOTOR_PWM, 0);
  } else {
    analogWrite(MOTOR_PWM, speed);
  }

  printStatus();
  delay(100);

}

// vi: syntax=arduino
