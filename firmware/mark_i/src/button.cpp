/*
 * button.cpp - Encapsulate the rather common task of checking a momentary
 * button/switch for a state change.
 */
#include <Arduino.h>
#include <limits.h>
#include "button.h"
#include "usec.h"
#include "debug.h"

#define SERIAL_DEBUG 0

Button::Button(int pin) {
  Button(pin, false);
}

// Pullup flips the response to the HIGH/LOW signals (for pull-up vs
// pull-down).
Button::Button(int pin, bool pullup) {
  Button(pin, pullup, 0);
}

Button::Button(int pin, bool pullup, unsigned long min_interval) {
  this->pin = pin;
  this->last = false;
  this->pullup = pullup;
  this->min_interval = min_interval;
  this->last_bounce = micros();

  if (!this->pullup) {
    pinMode(this->pin, INPUT);
  } else {
    pinMode(this->pin, INPUT_PULLUP);
  }
}

bool Button::check() {
  // Don't read twice as that could return different values, dontchaknow?
  int value = digitalRead(this->pin);
  unsigned long now = micros();
  bool retval = false;

#if SERIAL_DEBUG
  if (this->check_value(value)) {
    LOG("Button attached to pin " + String(this->pin) +
      " was pushed (last=") + String(this->last) +
      ", now=" + String(now) +
      ", last_bounce=" + String(this->last_bounce) + ")"
    );
  }
#endif

  // Is the button freshly being pressed? Compare to the appropriate voltage
  // based on the pullup parameter. Make sure we're actually transitioning
  // state.
  if (this->check_value(value) && !this->last && this->check_interval(now)) {
    this->last_bounce = now;
    this->last = true;
    retval = false; // have to wait for release, so still return false

  // Check if the button is freshly released by comparing to the OTHER voltage
  // and there's a state transition about to happen. This will ensure we're
  // doing a state transition.
  } else if (!this->check_value(value) && this->last && this->check_interval(now)) {
    this->last_bounce = now;
    this->last = false;
    retval = true;
  }

  return retval;
}

bool Button::check_interval(unsigned long now) {
  if (this->min_interval > 0) {
    return (bool)(USEC_DIFF(now, this->last_bounce) >= this->min_interval);
  } else {
    return true;
  }
}

bool Button::check_value(int value) {
  if (!this->pullup) {
    return (bool) (value == HIGH);
  } else {
    return (bool) (value == LOW);
  }
}
