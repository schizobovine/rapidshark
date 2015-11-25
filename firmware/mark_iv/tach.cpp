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
  this->times[pos] = micros();
  this->pos++;
}

/*
 * rpm() - Calculates current rolling average RPMs.
 */
uint16_t Tachometer::rpm() {
}

// vi: syntax=arduino
