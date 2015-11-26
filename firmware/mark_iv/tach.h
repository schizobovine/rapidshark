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

//
// Number of timestamps to keep on hand to calculate average. Since we're
// getting RAM limited, keep this small for now but still adjustable.
//
// NOTE Due to potential read-during-write issues with the multi-byte timer
// counts, only able to average HISTORY_SIZE-1 elements
//
const size_t HISTORY_SIZE = 5;

class Tachometer {
  
  public:
    Tachometer();
    void mark();
    float rpm();

  protected:

    // Timestamp of last measurement
    uint32_t last;

    // Measurements of elapsed time since last rotation
    uint32_t diffs[HISTORY_SIZE];

    // Current position for writing in the buffer; used mod buffer size so it's
    // always overwriting the oldest value. This position is NEVER safe to
    // read. However, the interrupt handler will change it before attempting to
    // write the next value.
    uint8_t pos = 0;

};

#endif
