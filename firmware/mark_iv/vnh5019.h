/*
 * vnh5019.h
 *
 * Library class to encapsulate motor handling for the VNH5019 H-bridge.
 *
 * Author: Sean "schizobovine" Caulfield <sean@yak.net>
 * License: GPLv2 (firmware) / CC4.0-BY-SA (documentation, hardware)
 *
 */

#ifndef __VNH5019_H
#define __VNH5019_H

#include <limits.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <Arduino.h>

//
// Motor States
//
// The motor's going to be in one of this set of states throughout our control
// over it. No, I don't care about the FWD<->REV change being missed. Ur dumb.
//
typedef enum {
  VNH5019_FREEWHEEL,
  VNH5019_GO,
  VNH5019_BRAKE,
  VNH5019_ACCEL,
} VNH5019_state_t;

//
// Controller object, one per motor instance (set of pins--probably could
// overlap if you need a few different instances on the same pins, but not
// tested or well thought out so probably ignore this)
//
class VNH5019 {

  public:

    VNH5019();
    VNH5019(int8_t a, int8_t b, int8_t pwm);
    VNH5019(int8_t a, int8_t b, int8_t pwm, uint8_t speed);

    void init();

    void go();
    void freewheel();
    void brake(); // Dumps heat into motor?

    uint8_t setSpeed(uint8_t new_speed);
    uint8_t getSpeed();

    void setPins(int8_t a, int8_t b, int8_t pwm);

    VNH5019_state_t getMotorState();

    bool isGoing();
    bool isFreewheeling();
    bool isBraking();

  protected:

    // High-level "state" of the motor
    volatile VNH5019_state_t motor_state;

    // Hard coding these because not exactly something to dick around with
    // much. Be sure these outputs are within the range of the motor!
    const uint8_t min_speed = 0;
    const uint8_t max_speed = 255;
    const uint8_t brake_speed = 255;
    
    // PWM duty cycle
    volatile uint8_t curr_speed;

    // Pins in use; using -1 as a guard for unset pin values
    int8_t pin_a = -1;
    int8_t pin_b = -1 ;
    int8_t pin_pwm = -1;

};

class VNH5019Pushit : VNH5019 {

  public:

    void pushit();
    void stopit_maybe();
    void stopit();

  private:

    volatile uint8_t target_speed;
    volatile uint16_t last_step;
    uint16_t interval = 10;

};

#endif
