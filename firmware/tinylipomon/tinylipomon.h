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

    TinyLipoMon(int serial_pin, int enable_pin, int num_cells);

    void begin();
    float voltage();
    float cell(uint8_t cell_num);
    uint16_t cell_raw(uint8_t cell_num);
    void set_divider(uint8_t cell_num, float value);

  protected:

    int serial_pin;
    int enable_pin;
    SoftwareSerial serial;
    uint8_t num_cells;
    uint16_t readings[];
    float vdividers[];
    float voltages[];

};

#endif
