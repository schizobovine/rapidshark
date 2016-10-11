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

uint16_t cell1_reading = 0;
uint16_t cell2_reading = 0;
uint16_t cell3_reading = 0;

SoftwareSerial serial(SERIAL_RX, SERIAL_TX);

////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// INTERRUPT HANDLERS
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// STARTUP CODE
////////////////////////////////////////////////////////////////////////

void setup() {
  pinMode(EN_DIV, OUTPUT);
  digitalWrite(EN_DIV, LOW);
  pinMode(SERIAL_CS, INPUT_PULLUP);
  serial.begin(SERIAL_BAUD);
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
    cell1_reading = analogRead(CELL_1);
    cell2_reading = analogRead(CELL_2);
    cell3_reading = analogRead(CELL_3);

    // Disable dividers by doing the reverse of the whole dance above. In
    // theory, you can take it back to tristate here, since the hardwired
    // "default" should be the safe/power-saving version that keeps all the
    // P-MOSFETs closed.
    digitalWrite(EN_DIV, LOW);

    //
    // Fart out data
    //

    serial.write((uint8_t)0x00);
    serial.write((uint8_t)((cell1_reading >> 0) & 0xFF));
    serial.write((uint8_t)((cell1_reading >> 8) & 0xFF));
    serial.write((uint8_t)0x01);
    serial.write((uint8_t)((cell2_reading >> 0) & 0xFF));
    serial.write((uint8_t)((cell2_reading >> 8) & 0xFF));
    serial.write((uint8_t)0x02);
    serial.write((uint8_t)((cell3_reading >> 0) & 0xFF));
    serial.write((uint8_t)((cell3_reading >> 8) & 0xFF));
    serial.write((uint8_t)0x03);
    serial.write((uint8_t)0xFF);
    serial.flush();

  }

  // Because I'm feeling sassy. And hate battery life (despite desiging this
  // whole circuit around it (don't judge my hypocracy! :P)).
  delay(100);

}

// vi: syntax=arduino
