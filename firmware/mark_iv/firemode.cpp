/*
 * firemode.h
 *
 * Library class to fire mode control state.
 *
 * Author: Sean "schizobovine" Caulfield <sean@yak.net>
 * License: GPLv2 (firmware) / CC4.0-BY-SA (documentation, hardware)
 *
 */

#include "firemode.h"

////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////////

FireMode::FireMode(fire_mode_t mode) {
  this->setMode(mode);
}

FireMode::FireMode() {
  FireMode(DEFAULT_MODE);
}


////////////////////////////////////////////////////////////////////////
// GETTERS & SETTERS
////////////////////////////////////////////////////////////////////////

fire_mode_t FireMode::getMode() {
  return this->_mode;
}

fire_mode_t FireMode::setMode(fire_mode_t mode) {
  this->_mode = mode;
  return this->_mode;
}

fire_mode_t FireMode::nextMode() {

  switch (this->getMode()) {
    case MODE_SEMI_AUTO:
      this->setMode(MODE_FULL_AUTO);
      break;
    case MODE_BURST:
      this->setMode(MODE_SEMI_AUTO);
      break;
    case MODE_FULL_AUTO:
      this->setMode(MODE_BURST);
      break;
    default:
      break;
  }

  return this->getMode();

}
