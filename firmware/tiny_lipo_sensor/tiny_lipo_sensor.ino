/*
 * tiny_lipo_sensor.ino
 *
 * Fart out battery stats over some kind of serial.
 *
 */

#include <Arduino.h>
#include <SoftwareSerial.h>

////////////////////////////////////////////////////////////////////////
// PIN ASSIGNMENT
////////////////////////////////////////////////////////////////////////

#define CELL_1 A0
#define CELL_2 A1
#define CELL_3 A2
#define EN_DIV 3

#define SERIAL_RX -1
#define SERIAL_TX 6
#define SERIAL_CS 4

#define SERIAL_BAUD 9600

////////////////////////////////////////////////////////////////////////
// GLOBAL STATE VARIABLES
////////////////////////////////////////////////////////////////////////

const uint8_t NUM_CELLS = 3;

const uint8_t CELL_PINS[NUM_CELLS] = {
  A0, A1, A2
};

//
//              R1 + R2
// Vin = Vout * -------
//                R2
//
// R1 = 4K7 (measured as 4.9K, 5% tolerance ugh)
//    = 22K1 (1%)
//    = 36K  (1%)
// R2 = 10K  (1%, same for all)
//

const float VDIV[NUM_CELLS] = {
  (( 4900.0 + 10000.0) / 10000.0),
  ((22100.0 + 10000.0) / 10000.0),
  ((36000.0 + 10000.0) / 10000.0),
};

uint16_t readings[NUM_CELLS] = {
  0, 0, 0
};

float cell_voltages[NUM_CELLS] = {
  0.0, 0.0, 0.0
};

SoftwareSerial serial(SERIAL_RX, SERIAL_TX);

////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
////////////////////////////////////////////////////////////////////////

float calc_voltage(uint8_t cell_num) {
  return (readings[cell_num] / 1024.0) * 5.0 * VDIV[cell_num];
}

////////////////////////////////////////////////////////////////////////
// STARTUP CODE
////////////////////////////////////////////////////////////////////////

void setup() {
  pinMode(EN_DIV, OUTPUT);
  digitalWrite(EN_DIV, LOW);
  pinMode(SERIAL_CS, INPUT_PULLUP);
}

////////////////////////////////////////////////////////////////////////
// MAIN LOOP
////////////////////////////////////////////////////////////////////////

void loop() {

  // Check chip select pin to see if we should do jack and/or shit
  if (digitalRead(SERIAL_CS) == LOW) {

    // Enable dividers by flipping Q4 (N-MOS) gate HIGH, thereby flipping the
    // gates for Q1, Q2 and Q3 (P-MOS) LOW, opening the connections to the cell
    // terminals.
    digitalWrite(EN_DIV, HIGH);

    // Wait a tick! The ADC needs time to stabilize.
    delay(10);

    // Do the needful
    for (uint8_t i=0; i<NUM_CELLS; i++) {
      readings[i] = analogRead(CELL_PINS[i]);
    }

    // Disable dividers by doing the reverse of the whole dance above. In
    // theory, you can take it back to tristate here, since the hardwired
    // "default" should be the safe/power-saving version that keeps all the
    // P-MOSFETs closed.
    digitalWrite(EN_DIV, LOW);

    // Voltage calculations
    //for (uint8_t i=0; i<NUM_CELLS; i++) {
    //  cell_voltages[i] = calc_voltage(i);
    //}

    //
    // Fart out data
    //
    // Wire protocol is pretty basic: cell number followed by two bytes of data
    // (raw 10bit ADC value plus 6bits padding), with 0xFF signalling end of
    // data.
    //
    // Byte# Value Comment
    //     0 0x01  Cell 1
    //     1 0x

    serial.begin(SERIAL_BAUD);
    serial.write((uint8_t)NUM_CELLS);
    for (uint8_t i=0; i<NUM_CELLS; i++) {
      serial.write((uint8_t)i); // Cell num
      serial.write((uint8_t)((readings[i] >> 0) & 0xFF));
      serial.write((uint8_t)((readings[i] >> 8) & 0xFF));
    }
    serial.write((uint8_t)0xFF);
    serial.flush();
    serial.end();

    // Because I'm feeling sassy. And hate battery life (despite desiging this
    // whole circuit around it (don't judge my hypocracy! :P)).
    delay(100);

  }

}

// vi: syntax=arduino
