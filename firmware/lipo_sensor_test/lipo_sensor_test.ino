/*
 * lipo_sensor_reader.ino - Verify output from battery sensor
 */

#include <Arduino.h>
#include <SPI.h>
#include <MicroView.h>
#include <Bounce2.h>
#include <stdlib.h>

#define NUM_CELLS 3

#define CELL1 A1
#define CELL2 A2
#define CELL3 A3
#define BUTT 2
#define LIPOMON_EN 3

#define VCC 5.0

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

const float VDIV[NUM_CELLS] = {
  (1.0 / 1024.0) * VCC * (( 4900.0 + 10000.0) / 10000.0),
  (1.0 / 1024.0) * VCC * ((22100.0 + 10000.0) / 10000.0),
  (1.0 / 1024.0) * VCC * ((36000.0 + 10000.0) / 10000.0),
};
const uint8_t CELLS[NUM_CELLS] = { CELL1, CELL2, CELL3 };

uint16_t readings[NUM_CELLS] = { 0, 0, 0 };
float values[NUM_CELLS] = { 0.0, 0.0, 0.0 };
Bounce butt;

void setup() {

  pinMode(LIPOMON_EN, OUTPUT);
  digitalWrite(LIPOMON_EN, LOW);

  butt.attach(BUTT, INPUT_PULLUP, 50);

  pinMode(CELL1, INPUT);
  pinMode(CELL2, INPUT);
  pinMode(CELL3, INPUT);

  uView.begin();
  uView.clear(ALL);
  uView.clear(PAGE);
  uView.display();

}

void loop() {

  if (butt.update() && butt.rose()) {
    uint8_t i;

    digitalWrite(LIPOMON_EN, HIGH);
    delay(10);

    for (i=0; i<NUM_CELLS; i++) {
      readings[i] = analogRead(CELLS[i]);
    }

    digitalWrite(LIPOMON_EN, LOW);

    for (i=0; i<NUM_CELLS; i++) {
      values[i] = readings[i] * VDIV[i];
    }

    uView.clear(ALL);
    for (i=0; i<NUM_CELLS; i++) {
      uView.setCursor(0, i*16);
      uView.print(i+1);
      uView.setCursor(12, i*16);
      uView.println(readings[i], DEC);
      uView.setCursor(12, i*16+8);
      uView.println(values[i], 2);
    }
    uView.display();

  }

}
