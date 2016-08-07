/*
 * tiny_motor_measurer.ino
 *
 * ATTiny85 sketch to measure voltage and current usage of a running motor and
 * dump the raw ADC info over a UART link piped through an optoisolator.
 * Current measurement performed by ACS732 (?) current transducer.
 *
 * Author: Sean Caulfield <sean@yak.net>
 * License: GPL v2.0
 *
 */

#include <Arduino.h>
#include <SoftwareSerial.h>

#define VOLTAGE_PIN A3
#define CURRENT_PIN A2
#define UART_TX_PIN 2
#define UART_BAUD 9600

SoftwareSerial uart = SoftwareSerial(-1, UART_TX_PIN);

uint32_t now = 0;
uint16_t raw_voltage = 0;
uint16_t raw_current = 0;

void setup() {
  pinMode(VOLTAGE_PIN, INPUT);
  pinMode(CURRENT_PIN, INPUT);
  pinMode(UART_TX_PIN, OUTPUT);
  uart.begin(UART_BAUD);
  uart.println(F("HELLO"));
}

void loop() {
  now = millis();
  raw_voltage = analogRead(VOLTAGE_PIN);
  raw_current = analogRead(CURRENT_PIN);
  uart.print(now);
  uart.print(F(","));
  uart.print(raw_voltage);
  uart.print(F(","));
  uart.println(raw_current);
}
