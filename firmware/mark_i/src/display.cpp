/*
 * display.cpp - Module to control a 2-digit, 7-segment display.
 *
 * Terminology:
 *   MSD = Most significant digit (in this case, the 10's digit)
 *   LSD = Least significant digit (in this case, the 1's digit)
 *
 * Parameters:
 *   seg_a_pin    \
 *   seg_b_pin    |
 *   seg_c_pin    |
 *   seg_d_pin    |
 *   seg_e_pin    +- Pins for 7 segment display control
 *   seg_f_pin    |  (See display.h for mapping)
 *   seg_g_pin    |
 *   decimal_pin  |
 *   msd_pin      |
 *   lsd_pin      /
 *   value     - Value currently being displayed
 *   last_flip - Time (in usec) that we last flipped PWM state
 *   pwm_state - Current PWM state
 *
 */

#include<Arduino.h>
#include "display.h"
#include "usec.h"

Display::Display( int seg_a_pin, int seg_b_pin, int seg_c_pin, int seg_d_pin,
    int seg_e_pin, int seg_f_pin, int seg_g_pin, int decimal_pin,
    int msd_pin, int lsd_pin,
    usec pwm_usec_off, usec pwm_usec_msd, usec pwm_usec_clr, usec pwm_usec_lsd) {

  this->seg_a_pin    = seg_a_pin;
  this->seg_b_pin    = seg_b_pin;
  this->seg_c_pin    = seg_c_pin;
  this->seg_d_pin    = seg_d_pin;
  this->seg_e_pin    = seg_e_pin;
  this->seg_f_pin    = seg_f_pin;
  this->seg_g_pin    = seg_g_pin;
  this->decimal_pin  = decimal_pin;
  this->msd_pin      = msd_pin;
  this->lsd_pin      = lsd_pin;
  this->value        = 0;
  this->last_flip    = 0;
  this->pwm_state    = PWM_STATE_OFF;
  this->pwm_usec_off = pwm_usec_off;
  this->pwm_usec_msd = pwm_usec_msd;
  this->pwm_usec_clr = pwm_usec_clr;
  this->pwm_usec_lsd = pwm_usec_lsd;

  PINMODE(this->seg_a_pin, OUTPUT);
  PINMODE(this->seg_b_pin, OUTPUT);
  PINMODE(this->seg_c_pin, OUTPUT);
  PINMODE(this->seg_d_pin, OUTPUT);
  PINMODE(this->seg_e_pin, OUTPUT);
  PINMODE(this->seg_f_pin, OUTPUT);
  PINMODE(this->seg_g_pin, OUTPUT);
  PINMODE(this->decimal_pin, OUTPUT);
  PINMODE(this->lsd_pin, OUTPUT);
  PINMODE(this->msd_pin, OUTPUT);

}

// Handles the main state checking/transitions
void Display::refresh() {
  unsigned int msd, lsd;
  usec now = micros();

  msd = (this->value / 10) % 10;
  lsd = this->value % 10;

  switch (this->pwm_state) {

    // "Off state" where the display should be clear. Sets all segment control
    // lines to LOW (meaning off) and the digit cathodes to HIGH (such that
    // they cannot sink current). The display should then be blank.
    case PWM_STATE_OFF:

      // Check for state transition
      if (this->timesUp(now, this->pwm_usec_off)) {
        this->pwm_state = PWM_STATE_MSD;
        this->last_flip = now;
      } else {
      }
      break;

    // "Most significant digit" display state. First, clear display by driving
    // all control segements low and both digit cathodes high. Then, set the
    // segment control lines for this digit, and finally set its cathode to LOW
    // so it can sink current (and thus display).
    case PWM_STATE_MSD:

      // Check for state transition
      if (this->timesUp(now, this->pwm_usec_msd)) {
        this->pwm_state = PWM_STATE_CLR;
        this->last_flip = now;
        this->clearSegments();
        digitalWrite(this->msd_pin, HIGH);
      } else {
        digitalWrite(this->msd_pin, LOW);
        this->writeDigit(msd);
      }
      break;

    // Second off state that acts as a pause between digits.
    case PWM_STATE_CLR:

      // Check for state transition
      if (this->timesUp(now, this->pwm_usec_clr)) {
        this->pwm_state = PWM_STATE_LSD;
        this->last_flip = now;
      } else {
      }
      break;

    // "Least significant digit" display state. First, clear display by driving
    // all control segements low and both digit cathodes high. Then, set the
    // segment control lines for this digit, and finally set its cathode to LOW
    // so it can sink current (and thus display).
    case PWM_STATE_LSD:

      // Check for state transition
      if (this->timesUp(now, this->pwm_usec_lsd)) {
        this->pwm_state = PWM_STATE_OFF;
        this->last_flip = now;
        this->clearSegments();
        digitalWrite(this->lsd_pin, HIGH);
      } else {
        digitalWrite(this->lsd_pin, LOW);
        this->writeDigit(lsd);
      }
      break;

    default: break; // wat
  }
}

void Display::put(int value) {
  this->value = value;
  this->refresh();
}

// Set pins according to the digit we want to display.
void Display::writeDigit(int digit) {
  int mask = LOOKUP_DIGIT_MASK(digit);

  DIGITAL_WRITE(this->seg_a_pin, (mask & SEG_A) ? HIGH : LOW);
  DIGITAL_WRITE(this->seg_b_pin, (mask & SEG_B) ? HIGH : LOW);
  DIGITAL_WRITE(this->seg_c_pin, (mask & SEG_C) ? HIGH : LOW);
  DIGITAL_WRITE(this->seg_d_pin, (mask & SEG_D) ? HIGH : LOW);
  DIGITAL_WRITE(this->seg_e_pin, (mask & SEG_E) ? HIGH : LOW);
  DIGITAL_WRITE(this->seg_f_pin, (mask & SEG_F) ? HIGH : LOW);
  DIGITAL_WRITE(this->seg_g_pin, (mask & SEG_G) ? HIGH : LOW);
  DIGITAL_WRITE(this->decimal_pin, (mask & SEG_P) ? HIGH : LOW);
}

// Clear all segments
void Display::clearSegments() {
  DIGITAL_WRITE(this->seg_a_pin, LOW);
  DIGITAL_WRITE(this->seg_b_pin, LOW);
  DIGITAL_WRITE(this->seg_c_pin, LOW);
  DIGITAL_WRITE(this->seg_d_pin, LOW);
  DIGITAL_WRITE(this->seg_e_pin, LOW);
  DIGITAL_WRITE(this->seg_f_pin, LOW);
  DIGITAL_WRITE(this->seg_g_pin, LOW);
  DIGITAL_WRITE(this->decimal_pin, LOW);
}

bool Display::timesUp(usec now, usec interval) {
  return USEC_DIFF(now, this->last_flip) >= interval;
}
