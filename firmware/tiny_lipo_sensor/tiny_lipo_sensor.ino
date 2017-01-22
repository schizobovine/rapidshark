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

const uint8_t NUM_CELLS = 3;
const uint8_t CELL_PINS[NUM_CELLS] = {
  A0, // cell 1
  A1, // cell 2
  A2  // cell 3
};

const uint8_t EN_DIV = 3;
const uint8_t OUTPUT_EN = 4; // PA4 aka SCLK aka SCL
const uint8_t SERIAL_RX = 5; // PA5 aka MISO
const uint8_t SERIAL_TX = 6; // PA6 aka MOSI aka SDA

#define SERIAL_BAUD 9600

//
// Voltage divider calculations
//
//              R1 + R2
// Vin = Vout * -------
//                R2
//
// R1 = 4K7  (1%)
//    = 22K1 (1%)
//    = 36K  (1%)
// R2 = 10K  (1%, same for all)
//

////////////////////////////////////////////////////////////////////////
// GLOBAL STATE VARIABLES
////////////////////////////////////////////////////////////////////////

// Pre-calculated array of voltage divider values
const float VDIV[NUM_CELLS] = {
  5.0 * (( 4900.0 + 10000.0) / 10000.0) / 1024.0,
  5.0 * ((22100.0 + 10000.0) / 10000.0) / 1024.0,
  5.0 * ((36000.0 + 10000.0) / 10000.0) / 1024.0,
};

uint16_t readings[NUM_CELLS] = {
  0, 0, 0
};

float voltages[NUM_CELLS] = {
  0.0, 0.0, 0.0
};

SoftwareSerial serial(SERIAL_RX, SERIAL_TX);

////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
////////////////////////////////////////////////////////////////////////

void read_voltage(uint8_t cell_num) {
  readings[cell_num] = analogRead(CELL_PINS[cell_num]);
  voltages[cell_num] = readings[cell_num] * VDIV[cell_num];
  return readings[cell_num];
}

////////////////////////////////////////////////////////////////////////
// STARTUP CODE
////////////////////////////////////////////////////////////////////////

void setup() {
  delay(200);
  pinMode(EN_DIV, OUTPUT);
  digitalWrite(EN_DIV, LOW);
  pinMode(OUTPUT_EN, INPUT_PULLUP);
  //pinMode(SERIAL_RX, INPUT);
  //pinMode(SERIAL_TX, OUTPUT);
}

////////////////////////////////////////////////////////////////////////
// MAIN LOOP
////////////////////////////////////////////////////////////////////////

void loop() {

  // Check chip select pin to see if we should do jack and/or shit
  if (digitalRead(OUTPUT_EN) == LOW) {

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
    //  voltages[i] = calc_voltage(i);
    //}

    //
    // Fart out data
    //
    // Wire protocol is pretty basic: cell number followed by two bytes of data
    // (raw 10bit ADC value plus 6bits padding), with 0xFF signalling end of
    // data.
    //

    serial.begin(SERIAL_BAUD);
    serial.write((uint8_t)NUM_CELLS);
    for (uint8_t i=0; i<NUM_CELLS; i++) {
      serial.write((uint8_t)(i+1)); // Cell num
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
