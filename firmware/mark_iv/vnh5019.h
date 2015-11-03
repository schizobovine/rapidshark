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

    VNH5019(uint8_t a, uint8_t b, uint8_t pwm);

    void go();
    void go(uint8_t speed);
    void stop();
    void brake();

    uint8_t setSpeed(uint8_t new_speed);
    uint8_t getSpeed();

  private:

    uint8_t pin_a;
    uint8_t pin_b;
    uint8_t pin_pwm;
    uint8_t curr_speed;

    // Hard coding these because not exactly something to dick around with
    // much. Be sure these outputs are within the range of the motor!
    const uint8_t min_speed = 0;
    const uint8_t max_speed = 64;

};

#endif
