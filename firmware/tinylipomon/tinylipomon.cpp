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

TinyLipoMon::TinyLipoMon(int _serial_pin, int _enable_pin, int _num_cells) {

  this->serial_pin = _serial_pin;
  this->enable_pin = _enable_pin;
  this->num_cells = _num_cells;

  this->serial = SoftwareSerial(this->serial_pin, -1);

  this->readings = new uint16_t[this->num_cells];
  this->voltages = new float[this->num_cells];
}

void TinyLipoMon::begin() {
  pinMode(this->enable_pin, OUTPUT);
  digitalWrite(this->enable_pin, HIGH);
}

float TinyLipoMon::voltage() {
  return this->voltages[this->num_cells - 1];
}

float TinyLipoMon::cell(int cell_num) {
  if (cell_num < this->num_cells) {
    return this->voltages[cell_num];
  }
}

uint16_t TinyLipoMon::cell_raw(int cell_num) {
  if (cell_num < this->num_cells) {
    return this->readings[cell_num];
  }
}

void TinyLipoMon::set_divider(uint8_t cell_num, float value) {
  if (cell_num < this->num_cells) {
    this->vdividers[cell_num] = value;
  }
}

bool TinyLipoMon::getReading() {

  // Signal to device to do some readings, then wait for data
  digitalWrite(this->enable_pin, LOW);
  this->serial.begin(TINYLIPOMON_SERIAL_BAUD);
  this->serial.setTimeout(TINYLIPOMON_SERIAL_TIMEOUT);

  // Attempt to connect, waiting for up to TINYLIPOMON_DELAY
  long started = millis();
  long now = started;
  while (!this->serial.available() && (now-started) < TINYLIPOMON_DELAY) {
    delay(1);
    now = millis();
  }
  if (!this->serial.available())
    return false;

  // Read values for cells
  uint8_t msb, lsb;
  uint8_t cells_read = 0;
  for (uint8_t i=0; i<this->num_cells; i++) {
    char c = this->serial.read();

    if (c == 0xFF) {
      break;
    } else if (c == (i+1)) {
      lsb = this->serial.read();
      msb = this->serial.read();
      this->readings[i] = (msb << 8) | (lsb);
      cells_read++;
    }
  }

  // Do conversion
  for (uint8_t i=0; i<this->num_cells; i++) {
    if (i < cells_read) {
      this->voltage[i] = this->readings[i] * this->vdividers[i];
    }
  }

}

// vi: syntax=arduino
