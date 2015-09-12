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
#include <EnableInterrupt.h>
#include <string.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

////////////////////////////////////////////////////////////////////////
// PIN ASSIGNMENT
////////////////////////////////////////////////////////////////////////

#define ENC_A      2
#define ENC_B      3
#define MOTOR_IN_A 5
#define MOTOR_PWM  6
#define MOTOR_IN_B 7
#define ENC_SW     8
#define LED_R      9
#define LED_G      10
#define LED_B      11

////////////////////////////////////////////////////////////////////////
// OTHER CONSTANTS
////////////////////////////////////////////////////////////////////////

// Hardware serial speed for debuggin'
#define BAUD_RATE 9600

// Bounds for PWM control. Arduino has about 10bits of PWM resolution, so we
// need to bound what the encoder frobulates by those values.
const int PWM_MIN = -1023;
const int PWM_MAX = 1023;

// Number of clicks ("detents" or pulses) per full rotation of the encoder
#define CLICKS_PER_ROTATION (24)

// User-definable number of rotations to hit full speed
#define MAX_ROTATIONS (1)

// How much PWM changes per detent of the encoder
const int LEVEL_PER_CLICK = (PWM_MAX + 1) / (CLICKS_PER_ROTATION * MAX_ROTATIONS);

////////////////////////////////////////////////////////////////////////
// GLOBAL STATE VARIABLES
////////////////////////////////////////////////////////////////////////

// Holds the current encoder state
volatile uint8_t enc_val = 0;
volatile uint8_t last_enc_val = 0;

// Holds the current speed state
volatile uint16_t speed = 0;

////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
////////////////////////////////////////////////////////////////////////

void printEncoderStatus() {
  PGM_P fmt = PSTR("%c %S");
  PGM_P val = NULL;
  char buff[16];
  char dir = '?';

  switch(enc_val) {
    case 0b0000: dir = '?'; val = PSTR("0000"); break;
    case 0b0001: dir = '?'; val = PSTR("0001"); break;
    case 0b0010: dir = '?'; val = PSTR("0010"); break;
    case 0b0011: dir = '?'; val = PSTR("0011"); break;
    case 0b0100: dir = '?'; val = PSTR("0100"); break;
    case 0b0101: dir = '?'; val = PSTR("0101"); break;
    case 0b0110: dir = '?'; val = PSTR("0110"); break;
    case 0b0111: dir = '?'; val = PSTR("0111"); break;
    case 0b1000: dir = '?'; val = PSTR("1000"); break;
    case 0b1001: dir = '?'; val = PSTR("1001"); break;
    case 0b1010: dir = '?'; val = PSTR("1010"); break;
    case 0b1011: dir = '?'; val = PSTR("1011"); break;
    case 0b1100: dir = '?'; val = PSTR("1100"); break;
    case 0b1101: dir = '?'; val = PSTR("1101"); break;
    case 0b1110: dir = '?'; val = PSTR("1110"); break;
    case 0b1111: dir = '?'; val = PSTR("1111"); break;
    default: break;
  }

  snprintf_P(buff, sizeof(buff), fmt, dir, val);
  Serial.println(buff);

}

////////////////////////////////////////////////////////////////////////
// INTERRUPT HANDLERS
////////////////////////////////////////////////////////////////////////

// In order to handle the encoder most properly, we need to setup interrupt
// handlers to interpret the Grey code output on change and manipulate the
// current PWM level accordingly. Note that these MUST return quickly because
// interrupts are typically blocked during handler execution to avoid
// overlapping handler runs and possible state loss; thus, only doing simple
// math in them and leaving the actual PWM change to the main loop.

//
// Grey code state change diagram
//
//         00 <-> 01
//          ^     ^
//          |     |
//          v     v
//         10 <-> 11
//
// 01 00 CCW
// 10 00 CW
//
// 00 01 CCW
// 11 01 CW
//
// 01 11 CCW
// 10 11 CW
//
// 00 10 CCW
// 11 10 CW
//
// CW  1000, 1011, 1101, 1110
// CCW 0111, 0100, 0010, 0001

// 00 10
//   old new
// A   0
// B

void intr_encoder() {
  uint8_t msb = digitalRead(ENC_A);
  uint8_t lsb = digitalRead(ENC_B);
  enc_val = (enc_val << 2) | (msb << 1) | lsb;
  enc_val = enc_val & 0x0F;
}

////////////////////////////////////////////////////////////////////////
// STARTUP CODE
////////////////////////////////////////////////////////////////////////

void setup() {

  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  pinMode(ENC_SW, INPUT_PULLUP);

  enableInterrupt(ENC_A, intr_encoder, CHANGE);
  enableInterrupt(ENC_B, intr_encoder, CHANGE);

  pinMode(MOTOR_IN_A, OUTPUT);
  pinMode(MOTOR_PWM, OUTPUT);
  pinMode(MOTOR_IN_B, OUTPUT);

  digitalWrite(MOTOR_IN_A, LOW);
  digitalWrite(MOTOR_IN_B, LOW);
  analogWrite(MOTOR_PWM, 0);

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, HIGH);

  Serial.begin(BAUD_RATE);
  Serial.println("HAI");

}

////////////////////////////////////////////////////////////////////////
// MAIN LOOP
////////////////////////////////////////////////////////////////////////

void loop() {
  uint8_t check_val = enc_val;

  if (last_enc_val != check_val) {
    //Serial.println(enc_val, BIN);
    printEncoderStatus();
    last_enc_val = check_val;
  }

}

// vi: syntax=arduino
