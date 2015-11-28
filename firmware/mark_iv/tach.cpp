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
Tachometer::Tachometer(uint8_t len) {
  this->len = len;
  this->diffs = new uint32_t[len];
  for (uint8_t i=0; i<this->len; i++) {
    this->diffs = 0;
  }
}

/*
 * ~Tachometer() - Destructors? Who even calls those?
 */
Tachometer::~Tachometer() {
  delete this->diffs;
}

/*
 * mark() - Called by IRQ handler for each beam break event to store the
 * current timestamp for later average calculation.
 */
void Tachometer::mark() {

  // We're entering new data so tag it for the bottom half handler
  this->new_data = true;

  // Increment first so reader won't attempt to read an incomplete value (since
  // only reader can be interrupted, not us).
  uint8_t old_pos = this->pos;
  this->pos = (this->pos + 1) % this->len;

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
  float sum_of_diff = 0;

  for (uint8_t i=0; i<this->len; i++) {
    if (i != pos) {
      sum_of_diff += this->diffs[i]; 
    }
  }

  float tau = sum_of_diff / (this->len - 1);
  float hz = 1000.0 * 1000.0 / tau;

  return hz * 60.0;
}

////////////////////////////////////////////////////////////////////////
// DEBUGGING
////////////////////////////////////////////////////////////////////////

/*
 * dump_serial() - Dump debugging data to serial port.
 */
#if 0
void Tachometer::dump_serial() {

  // With interrupts back on, we can now print out the debug data if it's fresh.
  if (new_data_copy) {
    for (uint8_t i=0; i<this->len; i++) {
      Serial.print(i, DEC);
      Serial.print(" ");
      Serial.println(this->diffs[i], DEC);
    }
    Serial.println();
  }

}
#endif

bool Tachometer::test_and_set() {

  // This bit of funkiness is to ensure the value we're pulling from our flag
  // variable can be overwritten by the irq handler while we're off doing other
  // stuff. Execution of the (relatively long) serial data dump depends only on
  // the local copy protected by the interrupt exclusion block (just long
  // enough to test, copy old value, reset shared var).

  bool new_data_copy = false;

  noInterrupts();
  if (this->new_data) {
    this->new_data = false;
    new_data_copy = true;
  }
  interrupts();

  return new_data_copy;

}

uint8_t Tachometer::getLen() {
  return this->len;
}

volatile uint32_t *Tachometer::getDiffs() {
  return this->diffs;
}

// vi: syntax=arduino
