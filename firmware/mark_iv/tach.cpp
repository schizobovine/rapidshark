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
Tachometer::Tachometer(uint8_t ignored) {
}

/*
 * ~Tachometer() - Destructors? Who even calls those?
 */
Tachometer::~Tachometer() {
}

/*
 * init() - Initialize sample array and other housekeeping shit.
 */
void Tachometer::init() {
  for (uint8_t i=0; i<this->num_samples; i++) {
    this->diffs[i] = 0;
  }
}

/*
 * mark() - Called by IRQ handler for each beam break event to store the
 * current timestamp for later average calculation.
 */
void Tachometer::mark() {

  // Ensure we've not been retriggered
  usec now = micros();
  if (now == this->last)
    return;

  // We're entering new data so tag it for the bottom half handler
  //this->new_data = true;

  // Increment first so reader won't attempt to read an incomplete value (since
  // only reader can be interrupted, not us).
  uint8_t old_pos = this->pos;
  this->pos = (this->pos + 1) % this->num_samples;

  // Save diff between now and last rotation in measurement buffer, then save
  // this timestamp.
  this->diffs[old_pos] = now - this->last;
  this->last = now;

}

/*
 * getDiffAt() - Retrieve diff value from buffer in a relatively irq friendly
 * way.
 *
 * If you attempt to access the current writing position, will return 0.
 *
 */
usec Tachometer::getDiffAt(uint8_t index) {
  usec copy = 0;

  // Bounds checking
  if (index < 0 || index >= this->num_samples)
    return 0;

  // Critical section
  noInterrupts();
  if (this->pos != index) {
    copy = this->diffs[index];
  }
  interrupts();

  // Return data copy
  return copy;

}

/*
 * rpm() - Calculates current rolling average RPMs.
 */
float Tachometer::rpm() {
  usec sum_of_diff = 0;
  uint8_t samples = 0;

  for (uint8_t i=0; i<this->num_samples; i++) {

    // Copy sample data in an interrupt safe way
    usec sample = this->getDiffAt(i);

    // If it's a valid sample, add to the running total
    if (sample != 0) {
      sum_of_diff += sample;
      samples++;
    }

  }

  // Check for divide by zero b/c that's not a great plan
  if (samples < 1)
    return 0.0;

  // Calculate average period over sample set
  float tau = ((float)sum_of_diff) / ((float)samples);

  // Convert to frequency
  float hz = 1000.0 * 1000.0 / tau;

  // Return RPMs
  return hz * 60.0;

}

////////////////////////////////////////////////////////////////////////
// DEBUGGING
////////////////////////////////////////////////////////////////////////

#if 0
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
#endif

// vi: syntax=arduino
