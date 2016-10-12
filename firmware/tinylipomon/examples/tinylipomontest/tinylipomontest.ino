/*
 * tinylipomontest.ino
 *
 * Simple library test for the tiny lipo battery monitor.
 *
 * Author: Sean Caulfield <sean@yak.net>
 * License: GPLv2
 *
 */

#include <Arduino.h>
#include <tinylipomon.h>

////////////////////////////////////////////////////////////////////////
// PIN ASSIGNMENT
////////////////////////////////////////////////////////////////////////

#define TINYLIPOMON_TX 11
#define TINYLIPOMON_EN 12

const uint8_t NUM_CELLS = 3;
const float VDIV[NUM_CELLS] = {
  (5.0 * 1024.0) * (( 4900.0 + 10000.0) / 10000.0),
  (5.0 * 1024.0) * ((22100.0 + 10000.0) / 10000.0),
  (5.0 * 1024.0) * ((36000.0 + 10000.0) / 10000.0),
};

////////////////////////////////////////////////////////////////////////
// GLOBAL STATE VARIABLES
////////////////////////////////////////////////////////////////////////

TinyLipoMon monitor = TinyLipoMon(TINYLIPOMON_TX, TINYLIPOMON_RX, NUM_CELLS);

////////////////////////////////////////////////////////////////////////
// STARTUP CODE
////////////////////////////////////////////////////////////////////////

void setup() {
  monitor.begin();
  for (uint8_t i=0; i<NUM_CELLS;i++) {
    monitor.set_divider(i, VDIV[i]);
  }
  while (!Serial)
    delay(10);
  Serial.begin(9600);
}

////////////////////////////////////////////////////////////////////////
// MAIN LOOP
////////////////////////////////////////////////////////////////////////

void loop() {
  for (uint8_t i=0; i<NUM_CELLS;i++) {
    Serial.print(F("Cell "));
    Serial.print(i);
    Serial.print(F(" = "));
    Serial.print(monitor.cell(i));
    Serial.print(F(", raw = "));
    Serial.print(monitor.cell_raw(i));
    Serial.println();
}
