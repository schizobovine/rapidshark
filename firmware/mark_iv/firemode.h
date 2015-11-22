/*
 * firemode.h
 *
 * Library class to fire mode control state.
 *
 * Author: Sean "schizobovine" Caulfield <sean@yak.net>
 * License: GPLv2 (firmware) / CC4.0-BY-SA (documentation, hardware)
 *
 */

#ifndef __FIREMODE_H
#define __FIREMODE_H

//
// Fire control states
//
typedef enum {
  MODE_SEMI_AUTO,
  MODE_BURST,
  MODE_FULL_AUTO,
} fire_mode_t;

const fire_mode_t DEFAULT_MODE = MODE_FULL_AUTO;

//
// Fire control state machine class
//
class FireMode {
  public:
    FireMode();
    FireMode(fire_mode_t mode);
    fire_mode_t getMode();
    fire_mode_t setMode(fire_mode_t mode);
    fire_mode_t nextMode();
  private:
    volatile fire_mode_t _mode;
};

#endif
