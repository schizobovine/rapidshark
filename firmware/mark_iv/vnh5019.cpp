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

////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////////

VNH5019::VNH5019() {
  this->curr_speed = 0;
  this->reverse = false;
}

VNH5019::VNH5019(int8_t a, int8_t b, int8_t pwm) {
  VNH5019();
  this->setPins(a, b, pwm);
} 

VNH5019::VNH5019(int8_t a, int8_t b, int8_t pwm, bool rev) {
  VNH5019(a, b, pwm);
  this->setReverse(rev);
}

////////////////////////////////////////////////////////////////////////
// INITIALIZATION
////////////////////////////////////////////////////////////////////////

// NB Don't want to put in the constructor since they'll (potentially) be doing
// pinMode and digitalWrites which probably causes havoc running before setup()
// gets called.

void VNH5019::init() {
  this->freewheel();
}

void VNH5019::init(int8_t a, int8_t b, int8_t pwm) {
  this->setPins(a, b, pwm);
  this->init();
}

void VNH5019::init(int8_t a, int8_t b, int8_t pwm, bool rev) {
  this->setReverse(rev);
  this->init(a, b, pwm);
}

////////////////////////////////////////////////////////////////////////
// COMMANDS
////////////////////////////////////////////////////////////////////////

/**
 * go() - Makes wheels go spiny
 */
void VNH5019::go() {

  // Set direction
  pinMode(this->pin_a, OUTPUT);
  pinMode(this->pin_b, OUTPUT);
  if (this->reverse) {
    digitalWrite(this->pin_a, LOW);
    digitalWrite(this->pin_b, HIGH);
  } else {
    digitalWrite(this->pin_a, HIGH);
    digitalWrite(this->pin_b, LOW);
  }

  // Set output speed
  pinMode(this->pin_pwm, OUTPUT);
  analogWrite(this->pin_pwm, this->curr_speed);


}

/**
 * go() - Makes wheels go spiny
 *
 * @param speed - Set PWM to this value before turning on motor(s)
 *
 */
void VNH5019::go(uint8_t speed) {
  this->setSpeed(speed);
  this->go();
}

/**
 * freewheel() - Sorta like stop, but by coasting instead of reverse polarity
 * breaking.
 */
void VNH5019::freewheel() {
  pinMode(this->pin_a, INPUT);
  pinMode(this->pin_b, INPUT);
  digitalWrite(this->pin_a, LOW);
  digitalWrite(this->pin_b, LOW);
  analogWrite(this->pin_pwm, 0);
}

/**
 * brake_vcc() - Stops motors going, dumping the generated power to VCC (and
 * theoretically charging the battery.
 */
void VNH5019::brake_vcc() {
  pinMode(this->pin_a, OUTPUT);
  pinMode(this->pin_b, OUTPUT);
  digitalWrite(this->pin_a, HIGH);
  digitalWrite(this->pin_b, HIGH);
  analogWrite(this->pin_pwm, this->curr_speed);
}

/**
 * brake_vcc() - Stops motors going, dumping the generated power to GND (and
 * thus using the motor as a heat-sink for the energy.
 */
void VNH5019::brake_gnd() {
  pinMode(this->pin_a, OUTPUT);
  pinMode(this->pin_b, OUTPUT);
  digitalWrite(this->pin_a, LOW);
  digitalWrite(this->pin_b, LOW);
  analogWrite(this->pin_pwm, this->curr_speed);
}

/**
 * brake() - Stops motors going, by default using braking to ground.
 */
void VNH5019::brake(bool regenerative=false) {
  if (regenerative) {
    this->brake_gnd();
  } else {
    this->brake_vcc();
  }
}

////////////////////////////////////////////////////////////////////////
// GETTERS & SETTERS
////////////////////////////////////////////////////////////////////////

uint8_t VNH5019::setSpeed(uint8_t new_speed) {
  this->curr_speed = constrain(new_speed, this->min_speed, this->max_speed);
  return this->curr_speed;
}

uint8_t VNH5019::getSpeed() {
  return this->curr_speed;
}

bool VNH5019::setReverse(bool rev) {
  this->reverse = rev;
  return this->reverse;
}

bool VNH5019::getReverse() {
  return this->reverse;
}

void VNH5019::setPins(int8_t a, int8_t b, int8_t pwm) {
  this->pin_a = a;
  this->pin_b = b;
  this->pin_pwm = pwm;
}

VNH5019_state_t VNH5019::getMotorState() {
  return this->motor_state;
}
