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
  this->motor_state = VNH5019_FREEWHEEL;
}

VNH5019::VNH5019(int8_t a, int8_t b, int8_t pwm) {
  VNH5019();
  this->setPins(a, b, pwm);
} 

VNH5019::VNH5019(int8_t a, int8_t b, int8_t pwm, uint8_t speed) {
  VNH5019();
  this->setPins(a, b, pwm);
  this->setSpeed(speed);
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

////////////////////////////////////////////////////////////////////////
// COMMANDS
////////////////////////////////////////////////////////////////////////

/**
 * go() - Makes wheels go spiny
 */
void VNH5019::go() {

  if (this->motor_state != VNH5019_GO) {
    this->motor_state = VNH5019_GO;

    pinMode(this->pin_a, OUTPUT);
    pinMode(this->pin_b, OUTPUT);
    digitalWrite(this->pin_a, HIGH);
    digitalWrite(this->pin_b, LOW);
    pinMode(this->pin_pwm, OUTPUT);
    analogWrite(this->pin_pwm, this->curr_speed);

  }

}

/**
 * freewheel() - Sorta like stop, but by coasting instead of reverse polarity
 * braking.
 */
void VNH5019::freewheel() {
  if (this->motor_state != VNH5019_FREEWHEEL) {
    this->motor_state = VNH5019_FREEWHEEL;
    pinMode(this->pin_a, INPUT);
    pinMode(this->pin_b, INPUT);
    digitalWrite(this->pin_a, LOW);
    digitalWrite(this->pin_b, LOW);
    analogWrite(this->pin_pwm, 0);
  }
}

/**
 * brake() - Stops motors going, dumping the generated power to GND (and thus
 * using the motor as a heat-sink for the energy.
 */
void VNH5019::brake() {
  if (this->motor_state != VNH5019_BRAKE) {
    this->motor_state = VNH5019_BRAKE;
    pinMode(this->pin_a, OUTPUT);
    pinMode(this->pin_b, OUTPUT);
    digitalWrite(this->pin_a, LOW);
    digitalWrite(this->pin_b, LOW);
    analogWrite(this->pin_pwm, this->brake_speed);
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

void VNH5019::setPins(int8_t a, int8_t b, int8_t pwm) {
  this->pin_a = a;
  this->pin_b = b;
  this->pin_pwm = pwm;
}

VNH5019_state_t VNH5019::getMotorState() {
  return this->motor_state;
}

bool VNH5019::isGoing() {
  return (this->motor_state == VNH5019_GO);
}

bool VNH5019::isFreewheeling() {
  return (this->motor_state == VNH5019_FREEWHEEL);
}

bool VNH5019::isBraking() {
  return (this->motor_state == VNH5019_BRAKE);
}
