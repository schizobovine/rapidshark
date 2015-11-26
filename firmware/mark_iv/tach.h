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

class Tachometer {
  
  public:

    Tachometer(uint8_t num_measures);
    ~Tachometer();
    void mark();
    float rpm();

  protected:

    // Timestamp of last measurement
    uint32_t last;

    // Measurements of elapsed time since last rotation
    uint32_t *diffs;

    // Current position for writing in the buffer; used mod buffer size so it's
    // always overwriting the oldest value. This position is NEVER safe to
    // read. However, the interrupt handler will change it before attempting to
    // write the next value.
    uint8_t pos = 0;

    // Number of timestamps to keep on hand to calculate average. Since we're
    // getting RAM limited, keep this small for now but still adjustable.
    uint8_t len = 0;

};

#endif
