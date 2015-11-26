/*
 * tach.cpp
 *
 * Keep track of flywheels rotational velocity.
 *
 * Author: Sean "schizobovine" Caulfield <sean@yak.net>
 * License: GPLv2 (firmware) / CC4.0-BY-SA (documentation, hardware)
 *
 */

#include <Arduino.h>
#include "tach.h"

/*
 * Tachometer() - Measures your tacos...I mean speed.
 */
Tachometer::Tachometer() {
}

/*
 * mark() - Called by IRQ handler for each beam break event to store the
 * current timestamp for later average calculation.
 */
void Tachometer::mark() {

  // Increment first so reader won't attempt to read an incomplete value (since
  // only reader can be interrupted, not us).
  uint8_t old_pos = this->pos;
  this->pos = (this->pos + 1) % HISTORY_SIZE;

  // Save diff between now and last rotation in measurement buffer, then save
  // this timestamp.
  uint32_t now = micros();
  this->diffs[old_pos] = now - last;
  this->last = now;
}

/*
 * rpm() - Calculates current rolling average RPMs.
 */
float Tachometer::rpm() {
  uint32_t sum_of_diff = 0;

  for (uint8_t i=0; i<HISTORY_SIZE; i++) {
    if (i != pos) {
      sum_of_diff += this->diffs[i]; 
    }
  }

  float tau = sum_of_diff / (HISTORY_SIZE - 1);
  float hz = 1000.0 * 1000.0 / tau;

  return hz * 60.0;
}

// vi: syntax=arduino
