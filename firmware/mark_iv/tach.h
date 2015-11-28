/*
 * tach.h
 *
 * Keep track of flywheels rotational velocity.
 *
 * Author: Sean "schizobovine" Caulfield <sean@yak.net>
 * License: GPLv2 (firmware) / CC4.0-BY-SA (documentation, hardware)
 *
 */

#ifndef __TACH_H
#define __TACH_H

typedef uint32_t usec;

class Tachometer {
  
  public:

    Tachometer(uint8_t num_measures);
    ~Tachometer();
    void init();
    void mark();
    float rpm();
    usec getDiffAt(uint8_t index);

    // Test are only used for debugging
    //bool test_and_set();

    // Total number of samples to store
    static const uint8_t num_samples = 4;

  protected:

    // Timestamp of last measurement
    volatile uint32_t last;

    // Measurements of elapsed time since last rotation
    volatile usec diffs[num_samples];

    // Current position for writing in the buffer; used mod buffer size so it's
    // always overwriting the oldest value. This position is NEVER safe to
    // read. However, the interrupt handler will change it before attempting to
    // write the next value.
    volatile uint8_t pos = 0;

    // Debugging use only
    //volatile bool new_data = false;

};

#endif
