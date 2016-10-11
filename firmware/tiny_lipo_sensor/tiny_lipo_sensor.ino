/*
 * tiny_lipo_sensor.ino
 *
 * Fart out battery stats over some kind of serial.
 *
 */

#include <Arduino.h>

////////////////////////////////////////////////////////////////////////
// PIN ASSIGNMENT
////////////////////////////////////////////////////////////////////////

#define CELL_1 A0
#define CELL_2 A1
#define CELL_3 A2
#define EN_DIV 3

#define OUT_0 6
#define OUT_1 4

////////////////////////////////////////////////////////////////////////
// GLOBAL STATE VARIABLES
////////////////////////////////////////////////////////////////////////

uint16_t cell1_reading = 0;
uint16_t cell2_reading = 0;
uint16_t cell3_reading = 0;

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
}

////////////////////////////////////////////////////////////////////////
// MAIN LOOP
////////////////////////////////////////////////////////////////////////

void loop() {

  // Enable dividers by flipping Q4 (N-MOS) gate HIGH, thereby flipping the
  // gates for Q1, Q2 and Q3 (P-MOS) LOW, opening the connections to the cell
  // terminals.
  digitalWrite(EN_DIV, HIGH);

  // Wait a tick! The ADC needs time to stabilize.
  delay(100);

  // Do the needful
  cell1_reading = analogRead(CELL_1);
  cell2_reading = analogRead(CELL_2);
  cell3_reading = analogRead(CELL_3);

  // Disable dividers by doing the reverse of the whole dance above. In theory,
  // you can take it back to tristate here, since the hardwired "default"
  // should be the safe/power-saving version that keeps all the P-MOSFETs
  // closed.
  digitalWrite(EN_DIV, LOW);

  // TODO
  // TODO Probably should tell something else about this stuff. OH WELL.
  // TODO

  // Because I'm feeling sassy. And hate battery life (despite desiging this
  // whole circuit around it (don't judge my hypocracy! :P)).
  delay(1000);

}

// vi: syntax=arduino
