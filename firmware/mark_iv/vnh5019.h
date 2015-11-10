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

class VNH5019 {

  public:

    VNH5019();
    VNH5019(int8_t a, int8_t b, int8_t pwm);
    VNH5019(int8_t a, int8_t b, int8_t pwm, bool rev);

    void init();
    void init(int8_t a, int8_t b, int8_t pwm);
    void init(int8_t a, int8_t b, int8_t pwm, bool rev);

    void go();
    void go(uint8_t speed);
    void freewheel();
    void brake_vcc(); // Charges battery?
    void brake_gnd(); // Dumps heat into motor?
    void brake(bool regenerative); // Dumps heat into motor?

    uint8_t setSpeed(uint8_t new_speed);
    uint8_t getSpeed();

    bool setReverse(bool newReverse);
    bool getReverse();

    void setPins(int8_t a, int8_t b, int8_t pwm);

  private:

    // Hard coding these because not exactly something to dick around with
    // much. Be sure these outputs are within the range of the motor!
    const uint8_t min_speed = 0;
    const uint8_t max_speed = 64;

    // Pins in use; using -1 as a guard for unset pin values
    int8_t pin_a = -1;
    int8_t pin_b = -1 ;
    int8_t pin_pwm = -1;
    
    uint8_t curr_speed;
    bool reverse = false;

};

#endif
