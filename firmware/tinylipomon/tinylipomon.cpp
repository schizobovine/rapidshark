/*
 * tinylipomon.cpp
 *
 * Lithium polymer battery balance monitor library.
 *
 * Author: Sean Caulfield <sean@yak.net>
 * License: GPLv2
 *
 */

#include "tinylipomon.h"

TinyLipoMon::TinyLipoMon(uint8_t _serial_pin, uint8_t _enable_pin, uint8_t _num_cells) {

  this->serial_pin = _serial_pin;
  this->enable_pin = _enable_pin;
  this->num_cells = _num_cells;

  this->serial = new SoftwareSerial(this->serial_pin, -1);

  this->readings = new uint16_t[this->num_cells];
  this->voltages = new float[this->num_cells];

}

void TinyLipoMon::begin() {
  pinMode(this->enable_pin, OUTPUT);
  digitalWrite(this->enable_pin, HIGH);
}

// Get "combined" total voltage of battery array, assumed to be the voltage
// between the positive terminal of the final cell in series vs. the negative
// terminal of the first cell: (-) and (+) for the entire battery.
float TinyLipoMon::voltage() {
  if (this->num_cells > 0) {
    return this->voltages[this->num_cells - 1];
  } else {
    return 0;
  }
}

// Get voltage for particular cell, as compared to the previous cell. This
// subtracts out the values of intervening cells to get this value. For the
// measure vs the ground/firt cell (-) terminal, see cell().
float TinyLipoMon::cell_exact(uint8_t cell_num) {
  if (this->num_cells > 0 && cell_num < this->num_cells) {
    float value = this->voltages[cell_num];
    for (uint8_t i=cell_num-1; i>=0; i--) {
      value -= this->voltages[i];
    }
    return value;
  } else {
    return 0;
  }
}

// Get voltage for particular cell. Note that this is measured against ground,
// NOT between this cell and its neighbor. For that, see cell_exact().
float TinyLipoMon::cell(uint8_t cell_num) {
  if (cell_num < this->num_cells) {
    return this->voltages[cell_num];
  } else {
    return 0;
  }
}

// Get raw ADC value from a particular cell
uint16_t TinyLipoMon::cell_raw(uint8_t cell_num) {
  if (cell_num < this->num_cells) {
    return this->readings[cell_num];
  } else {
    return 0;
  }
}

// Set voltage divider value, i.e., the value we multiply the ADC output by to
// get a floating point voltage representation.
void TinyLipoMon::set_divider(uint8_t cell_num, float value) {
  if (cell_num < this->num_cells) {
    this->vdividers[cell_num] = value;
  }
}

//
// Take difference between two outputs of millis(), accounding for integer
// rollover.
//
// NB: This operation, because it returns another unsigned integer, is NOT
// associative. I.e., this is the distance one would have to travel on a number
// ring to get FROM a TO b. If it turns out a is GREATER THAN b, we got a
// wrap-around, and so give a difference based on "travelling" from a to
// ULONG_MAX, back to 0 and forward to b.
//
uint32_t TinyLipoMon::usec_diff(unsigned uint32_t a, unsigned uint32_t b) {
  if (a > b) {
    return (ULONG_MAX - a) + b;
  } else {
    return b - a;
  }
}

// Have we passed the time-out threshold?
bool TinyLipoMon::timedout(uint32_t start) {
  return this->usec_diff(millis(), start) > TINYLIPOMON_DELAY;
}

// Poke at the device to get some data. Returns true on success, false on
// falure.
bool TinyLipoMon::getReading() {

  // Start up software serial
  this->serial->begin(TINYLIPOMON_SERIAL_BAUD);
  this->serial->setTimeout(TINYLIPOMON_SERIAL_TIMEOUT);

  // Signal to device to do some readings, then wait for data
  digitalWrite(this->enable_pin, LOW);

  // Attempt to connect, waiting for up to TINYLIPOMON_DELAY
  uint32_t start = millis();
  while (!this->serial->available() && !this->timedout(start)) {
    delay(1);
  }

  // Timed out, give up
  if (!this->serial->available())
    return false;

  // Read values for cells
  uint8_t msb, lsb;
  uint8_t cells_read = 0;
  for (uint8_t i=0; i<this->num_cells; i++) {
    uint8_t c = this->serial->read();

    // Signals data done
    if (c == 0xFF) {
      break;
    } else if (c == (i+1)) {
      lsb = this->serial->read();
      msb = this->serial->read();
      this->readings[i] = (msb << 8) | (lsb);
      cells_read++;
    }
  }

  // Do conversion
  for (uint8_t i=0; i<this->num_cells; i++) {
    if (i < cells_read) {
      this->voltages[i] = this->readings[i] * this->vdividers[i];
    }
  }

  return true;
}

// vi: syntax=arduino
