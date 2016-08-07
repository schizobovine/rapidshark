/*
 * tiny_motor_measurer.ino
 *
 * Arduino sketch to display/log raw ADC data piped through the optoisolated
 * UART link to gather data on motor performance.
 *
 * Author: Sean Caulfield <sean@yak.net>
 * License: GPL v2.0
 *
 */

#include <Arduino.h>
#include <SoftwareSerial.h>

#define UART_RX_PIN 2
#define UART_RX_BAUD 9600
#define UART_TX_BAUD 115200
#define uart_tx Serial

SoftwareSerial uart_rx = SoftwareSerial(UART_RX_PIN, -1);

void setup() {
  pinMode(UART_RX_PIN, INPUT);
  uart_rx.begin(UART_RX_BAUD);
  uart_tx.begin(UART_TX_BAUD);
  uart_tx.println(F("HELLO DAVE"));
}

void loop() {
  while (uart_rx.available()) {
    uart_tx.write(uart_rx.read());
  }
}
