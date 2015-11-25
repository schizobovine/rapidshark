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
const size_t HISTORY_SIZE = 4;

class Tachometer {
  
  public:
    Tachometer();
    void mark();
    uint16_t rpm();

  protected:

    // Last X timestamps for calculating RPM
    uint16_t times[HISTORY_SIZE];

    // Current position for writing in the buffer; used mod buffer size so it's
    // always overwriting the oldest value.
    uint8_t pos = 0;

};

#endif
