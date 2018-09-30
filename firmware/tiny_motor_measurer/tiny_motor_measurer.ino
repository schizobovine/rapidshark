/*
 * tiny_motor_measurer.ino
 *
 * Sketch to measure voltage and current usage of a running motor and dump the
 * raw ADC info over a UART link piped through an optoisolator. Current
 * measurement performed by ACS732 (?) current transducer.
 *
 * Author: Sean Caulfield <sean@yak.net>
 * License: GPL v2.0
 *
 */

#include <Arduino.h>
#include <Bounce2.h>
#include <SD.h>

#define VOLTAGE_PIN A3
#define CURRENT_PIN A2
#define BUTT_PIN 2
#define LED_PIN 4
#define SD_CS 10
//#define HS_PIN 5
#define LS_PIN 6

#define LOGFILE "motor.log"
#define LOGMODE (O_WRITE|O_CREAT|O_APPEND)

#define DEBOUNCE_MS 100

File logfile;
Bounce butt;

uint32_t now = 0;
uint16_t raw_voltage = 0;
uint16_t raw_current = 0;

boolean logging = false;

void error(const char *msg) {
  Serial.println(msg);
  while (true)
    delay(1000);
}

void shart_data(Stream &out) {
  out.print(now);
  out.print(F(","));
  out.print(raw_voltage);
  out.print(F(","));
  out.println(raw_current);
}

void setup() {

  pinMode(VOLTAGE_PIN, INPUT);
  pinMode(CURRENT_PIN, INPUT);

  //pinMode(HS_PIN, OUTPUT);
  //digitalWrite(HS_PIN, HIGH);
  pinMode(LS_PIN, OUTPUT);
  digitalWrite(LS_PIN, LOW);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  butt.attach(BUTT_PIN, INPUT_PULLUP, DEBOUNCE_MS);

  pinMode(SD_CS, OUTPUT);
  if (!SD.begin(SD_CS)) {
    error("MicroSD card not found!");
  }
}

void start_test() {
  Serial.begin(115200);
  if (!(logfile = SD.open(LOGFILE, LOGMODE))) {
    error("Unable to open logfile " LOGFILE "!");
  }
  //digitalWrite(HS_PIN, LOW);
  //digitalWrite(LS_PIN, HIGH);
  analogWrite(LS_PIN, 64);
}

void stop_test() {
  //digitalWrite(HS_PIN, HIGH);
  digitalWrite(LS_PIN, LOW);
  Serial.flush();
  Serial.end();
  logfile.flush();
  logfile.close();
}

void loop() {

  if (butt.update()) {
    if (butt.fell()) {
      if (logging) {
        logging = false;
        digitalWrite(LED_PIN, HIGH);
        stop_test();
      } else {
        logging = true;
        digitalWrite(LED_PIN, LOW);
        start_test();
      }
    }
  }

  if (logging) {

    now = millis();
    raw_voltage = analogRead(VOLTAGE_PIN);
    raw_current = analogRead(CURRENT_PIN);

    shart_data(Serial);
    shart_data(logfile);

  }

}
