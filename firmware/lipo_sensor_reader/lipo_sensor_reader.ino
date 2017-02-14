/*
 * lipo_sensor_reader.ino - Verify output from battery sensor
 */

#include <Arduino.h>
#include <SPI.h>
#include <MicroView.h>
#include <stdlib.h>

#define LIPOMON_EN  3
#define LIPOMON_DAT 2
#define LIPOMON_CLK 5
#define BUTT A1

#define LIPOMON_VCC 5.0
#define LIPOMON_ADC 1024.0

#define NUM_CELLS 3
#define SERIAL_BAUD 9600
#define PREC 2

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

// Precalculate scaling constants for ADC outputs
const float VDIV[NUM_CELLS] = {
  (1.0 / 1024.0) * 5.0 * (( 4900.0 + 10000.0) / 10000.0),
  (1.0 / 1024.0) * 5.0 * ((22100.0 + 10000.0) / 10000.0),
  (1.0 / 1024.0) * 5.0 * ((36000.0 + 10000.0) / 10000.0),
};

//SoftwareSerial serial(LIPOMON_RX, LIPOMON_TX);

uint8_t msb, lsb;
uint16_t readings[NUM_CELLS];
uint8_t buff[NUM_CELLS*2] = {
  1, 2, 3, 4, 5, 6
};
float voltage;

float calc_voltage(uint16_t adc_reading, uint8_t cell) {
  return VDIV[cell] * adc_reading;
}

void setup() {
  delay(200);
  pinMode(LIPOMON_DAT, INPUT);
  pinMode(LIPOMON_CLK, OUTPUT);
  pinMode(LIPOMON_EN, OUTPUT);
  pinMode(BUTT, INPUT_PULLUP);
  digitalWrite(LIPOMON_EN, LOW);
  uView.begin();
  uView.clear(ALL);
  uView.clear(PAGE);
  uView.display();
}

void loop() {

  // Enable monitoring
  //digitalWrite(LIPOMON_EN, HIGH);
  if (digitalRead(BUTT) == LOW) {

    // Wait a tick for it to crunch the numbers
    uView.clear(ALL);
    uView.setCursor(0, 0);
    uView.print(F("READING"));
    uView.display();

    // Get data
    for (uint8_t i=0; i<NUM_CELLS*2; i++) {
      buff[i] = shiftIn(LIPOMON_DAT, LIPOMON_CLK, MSBFIRST);
      //msb = shiftIn(LIPOMON_DAT, LIPOMON_CLK, MSBFIRST);
      //delay(1);
      //lsb = shiftIn(LIPOMON_DAT, LIPOMON_CLK, MSBFIRST);
      //readings[i] = (msb << 8) | lsb;
    }

    // Disable monitoring
    digitalWrite(LIPOMON_EN, LOW);

    // Print data to display
    uView.clear(PAGE);
    for (uint8_t i=0; i<NUM_CELLS*2; i++) {
      //uView.print(readings[i], HEX);
      //if (i % 2 == 0) {
      //  uView.setCursor(0,  (i/2)*8);
      //} else {
      //  uView.setCursor(16, (i/2)*8);
      //}
      uView.setCursor(0, i*8);
      uView.print(buff[i], BIN);
      //uView.print(buff[2*i+1], HEX);
    }
    uView.display();

    delay(1000);

  }

  // Connect over serial
  //serial.begin(SERIAL_BAUD);

  //// Wait for data
  //while (!serial.available()) {
  //  delay(10);
  //}

  //// Check for number of cells to match
  //char c = serial.read();
  //if (serial.read() == NUM_CELLS) {
  //  for (uint8_t i=0; i<NUM_CELLS; i++) {
  //    msb = serial.read();
  //    lsb = serial.read();
  //    reading = (msb << 8) + lsb;
  //    voltage = calc_voltage(reading, i);
  //    uView.print(i, DEC);
  //    uView.print(" ");
  //    uView.print(reading, HEX);
  //    uView.print(" ");
  //    uView.println(voltage, PREC);
  //  }
  //  uView.display();
  //} else {
  //  uView.print("bad # cells");
  //  uView.display();
  //}

  //delay(1000);

}
