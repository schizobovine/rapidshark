/*
 * tinylipomon.h
 *
 * Lithium polymer battery balance monitor library.
 *
 * Author: Sean Caulfield <sean@yak.net>
 * License: GPLv2
 *
 */

#include <Arduino.h>
#include <SoftwareSerial.h>

#ifndef _TINYLIPOMON_H
#define _TINYLIPOMON_H

#define TINYLIPOMON_SERIAL_BAUD 9600
#define TINYLIPOMON_SERIAL_TIMEOUT 100
#define TINYLIPOMON_DELAY 20

class TinyLipoMon {

  public:

    TinyLipoMon(uint8_t serial_pin, uint8_t enable_pin, uint8_t num_cells);

    void begin();
    float voltage();
    float cell(uint8_t cell_num);
    uint16_t cell_raw(uint8_t cell_num);
    void set_divider(uint8_t cell_num, float value);
    bool getReading();

  protected:

    bool keep_waiting_for_data();
    bool usec_diff(unsigned long a, unsigned long b);
    uint32_t start_time;
    uint8_t serial_pin;
    uint8_t enable_pin;
    SoftwareSerial *serial;
    uint8_t num_cells;
    uint16_t *readings;
    float *vdividers;
    float *voltages;

};

#endif
