/*
 * lipo_sensor_reader.ino - Verify output from battery sensor
 */

#include <Arduino.h>
#include <SPI.h>
#include <MicroView.h>
#include <SoftwareSerial.h>

#define LIPOMON_RX A0
#define LIPOMON_TX A1
#define LIPOMON_EN A2

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

SoftwareSerial serial(LIPOMON_RX, LIPOMON_TX);

uint8_t msb, lsb;
uint16_t reading;
float voltage;

float calc_voltage(uint16_t adc_reading, uint8_t cell) {
  return VDIV[cell] * adc_reading;
}

void setup() {
  delay(200);
  pinMode(LIPOMON_RX, INPUT);
  pinMode(LIPOMON_TX, OUTPUT);
  pinMode(LIPOMON_EN, OUTPUT);
  digitalWrite(LIPOMON_EN, HIGH);
  uView.begin();
  uView.clear(ALL);
  uView.clear(PAGE);
  uView.display();
}

void loop() {

  // Enable monitoring
  digitalWrite(LIPOMON_EN, LOW);

  // Connect over serial
  serial.begin(SERIAL_BAUD);

  // Wait for data
  while (!serial.available()) {
    delay(10);
  }

  // Clear display
  uView.clear(PAGE);

  // Check for number of cells to match
  char c = serial.read();
  if (serial.read() == NUM_CELLS) {
    for (uint8_t i=0; i<NUM_CELLS; i++) {
      msb = serial.read();
      lsb = serial.read();
      reading = (msb << 8) + lsb;
      voltage = calc_voltage(reading, i);
      uView.print(i, DEC);
      uView.print(" ");
      uView.print(reading, HEX);
      uView.print(" ");
      uView.println(voltage, PREC);
    }
    uView.display();
  } else {
    uView.print("bad # cells");
    uView.display();
  }

  // Disable monitoring
  digitalWrite(LIPOMON_EN, HIGH);

  delay(1000);

}
