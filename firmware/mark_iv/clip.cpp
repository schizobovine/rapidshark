/*
 * clip.cpp
 *
 * Library class for ammo clip management
 *
 * Author: Sean "schizobovine" Caulfield <sean@yak.net>
 * License: GPLv2 (firmware) / CC4.0-BY-SA (documentation, hardware)
 *
 */

#include "clip.h"

Clip::Clip(clip_type_t type) {
  this->setType(type);
}

Clip::Clip() {
  Clip(CLIP_DEFAULT);
}

clip_type_t Clip::getType() {
  return this->_type;
}

clip_type_t Clip::setType(clip_type_t type) {

  this->_type = type;
  this->ammo_max = CLIP_TYPE_TO_MAX_COUNT(this->_type);
  this->ammo_current = this->ammo_max;

  return this->_type;
}

clip_type_t Clip::nextType() {

  switch (this->getType()) {
    case CLIP_TYPE_18:
      this->setType(CLIP_TYPE_36);
      break;
    case CLIP_TYPE_36:
      this->setType(CLIP_TYPE_18);
      break;
    default:
      this->_type = CLIP_DEFAULT;
      break;
  }

  return this->_type;
}

uint8_t Clip::getMax() {
  return this->ammo_max;
}

uint8_t Clip::getCurrent() {
  return this->ammo_current;
}

uint8_t Clip::decrement() {
  if (this->ammo_current > 0) {
    this->ammo_current--;
  }
  return this->ammo_current;
}

uint8_t Clip::reset() {
  this->ammo_current = this->ammo_max;
  return this->ammo_current;
}
