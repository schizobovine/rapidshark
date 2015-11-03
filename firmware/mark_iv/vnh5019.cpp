/*
 * vnh5019.cpp
 *
 * Library class to encapsulate motor handling for the VNH5019 H-bridge.
 *
 * Author: Sean "schizobovine" Caulfield <sean@yak.net>
 * License: GPLv2 (firmware) / CC4.0-BY-SA (documentation, hardware)
 *
 */

#include "vnh5019.h"

VNH5019::VNH5019(uint8_t a, uint8_t b, uint8_t pwm) {
  this->pin_a = a;
  this->pin_b = b;
  this->pin_pwm = pwm;
  this->curr_speed = 0;
}

void VNH5019::go() {
  digitalWrite(this->pin_a, HIGH);
  digitalWrite(this->pin_b, LOW);
  analogWrite(this->pin_pwm, this->curr_speed);
}

void VNH5019::go(uint8_t speed) {
  this->setSpeed(speed);
  this->go();
}

void VNH5019::stop() {
  //digitalWrite(this->pin_a, HIGH);
  //digitalWrite(this->pin_b, LOW);
  analogWrite(this->pin_pwm, 0);
}

void VNH5019::brake() {
  digitalWrite(this->pin_a, LOW);
  digitalWrite(this->pin_b, LOW);
  analogWrite(this->pin_pwm, this->curr_speed);
}

uint8_t VNH5019::setSpeed(uint8_t new_speed) {
  this->curr_speed = constrain(new_speed, this->min_speed, this->max_speed);
  return this->curr_speed;
}

uint8_t VNH5019::getSpeed() {
  return this->curr_speed;
}
