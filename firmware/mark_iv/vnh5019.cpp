/*
 * vnh5019.cpp
 *
 * Library class to encapsulate motor handling for the VNH5019 H-bridge.
 *
 * Author: Sean "schizobovine" Caulfield <sean@yak.net>
 * License: GPLv2 (firmware) / CC4.0-BY-SA (documentation, hardware)
 *
 */

#include <Arduino.h>
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
  pinMode(this->pin_a, OUTPUT);
  pinMode(this->pin_b, OUTPUT);
  pinMode(this->pin_pwm, OUTPUT);
  this->brake();
}

////////////////////////////////////////////////////////////////////////
// COMMANDS
////////////////////////////////////////////////////////////////////////

/**
 * go() - Makes wheels go spiny
 */
void VNH5019::go() {
  if (this->motor_state != VNH5019_GO) {
    this->stopit_maybe();
    this->motor_state = VNH5019_GO;
    digitalWrite(this->pin_a, HIGH);
    digitalWrite(this->pin_b, LOW);
    analogWrite(this->pin_pwm, this->curr_speed);
  }
}

/**
 * brake() - Stops motors going, dumping the generated power to GND (and thus
 * using the motor as a heat-sink for the energy.
 */
void VNH5019::brake() {
  if (this->motor_state != VNH5019_BRAKE) {
    this->stopit_maybe();
    this->motor_state = VNH5019_BRAKE;
    digitalWrite(this->pin_a, LOW);
    digitalWrite(this->pin_b, LOW);
    analogWrite(this->pin_pwm, this->brake_speed);
  }
}

/**
 * pushit() - Gotta go fast. Pushes PWM to maximum at first, then backs off by
 * half after an interval has passed (default 1ms).
 */
void VNH5019::pushit() {

  // First time in the accerlation mode, stepup required
  if (this->motor_state != VNH5019_ACCEL) {

    this->motor_state = VNH5019_ACCEL;
    this->target_speed = this->curr_speed;
    this->curr_speed = 255;
    this->last_step = millis();

    digitalWrite(this->pin_a, HIGH);
    digitalWrite(this->pin_b, LOW);
    analogWrite(this->pin_pwm, this->curr_speed);

  // Re-called to set state, so check our last step and reduce PWM if needed.
  } else {
    uint16_t now = millis();

    // We've had at least one beat since last check, so drop PWM
    if ((this->last_step - now) >= this->interval) {
      uint8_t diff;

      diff = this->curr_speed - this->target_speed;
      diff = max(diff/2, 1);
      this->curr_speed = this->curr_speed - diff;

      // If we've reached the target speed, switch over to regular go mode
      if (this->curr_speed <= this->target_speed) {
        this->curr_speed = this->target_speed;
        this->go();

      // Otherwise, reduce speed and remember the time
      } else {
        this->last_step = now;
        analogWrite(this->pin_pwm, this->curr_speed);
      }

    }

  }

}

/**
 * stopit_maybe() - Resets any special state from the acceleration when
 * entering another mode, first checking if we're in that state.
 */
void VNH5019::stopit_maybe() {
  if (this->motor_state == VNH5019_ACCEL) {
    this->curr_speed = this->target_speed;
    this->last_step = 0;
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
