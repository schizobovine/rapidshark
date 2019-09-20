/*
 * counter.cpp - Simple utility class for keeping track of a counter, including
 * setting soft & hard bounds on said counter (ignoring attempts to read
 * outside that range), and providing tools for adjusting those.
 *
 * Notable parameters:
 *
 * soft_min, soft_max - Current "soft" limits, which limit the counter's
 * current value range. Can be manipulated by increasing or decreasing the
 * appropriate limit variables.
 *
 * hard_min, hard_max - Instance's "hard" limits. These are set at instance
 * creation time and cannot be changed during the life of the instance.
 *
 * value - Current counter value. If provided at instance creation time,
 * defines the first value the instance will take on. Otherwise, it defaults to
 * 0. Cannot be outside the hard bounds (and will be silently forced into those
 * bounds if not).
 */

#include <Arduino.h>
#include "counter.h"

Counterator::Counterator(int min, int max, int value) {
  Counterator(min, max, min, max, value);
}

Counterator::Counterator(int soft_min, int soft_max, int hard_min, int hard_max, int value) {
  /*
  if (soft_min > soft_max || hard_min > hard_max || hard_min > soft_max || soft_min > hard_max) {
    throw new Exception();
  }
  */
  this->soft_min = soft_min;
  this->soft_max = soft_max;
  this->hard_min = hard_min;
  this->hard_max = hard_max;
  this->value = value;
}

int Counterator::getSoftMin(){
  return this->soft_min;
}

int Counterator::setSoftMin(int new_soft_min){
  // Bound the soft minimum by the hard limits. This ensures we only have to
  // compare against the soft limits, since we know they're already inside the
  // hard limits.
  this->soft_min = MIN(this->hard_max, MAX(new_soft_min, this->hard_min));

  return this->soft_min;
}

int Counterator::getSoftMax(){
  return this->soft_max;
}

int Counterator::setSoftMax(int new_soft_max){
  // Bound the soft maximum by the hard limits. This ensures we only have to
  // compare against the soft limits, since we know they're already inside the
  // hard limits.
  this->soft_max = MIN(this->hard_max, MAX(new_soft_max, this->hard_min));

  // Knock down the value to be in line with the current max
  if (this->value > this->soft_max) {
    this->value = this->soft_max;
  } else if (this->value < this->soft_min) {
    this->value = this->soft_max;
  }

  return this->soft_max;
}

int Counterator::getHardMin(){
  return this->hard_min;
}

int Counterator::getHardMax(){
  return this->hard_max;
}

int Counterator::getValue(){
  return this->value;
}

int Counterator::setValue(int newValue) {

  if (newValue >= this->soft_min && newValue <= this->soft_max) {
    this->value = newValue;
  }

  return this->value;
}

int Counterator::increment(){
  // Bound the counter by soft limits. We don't have to check the hard limits
  // since they should already be within bounds. We don't need to check the
  // minimums, either, since INTEGER OVERFLOW CANNOT POSSIBLY HAPPEN. >_>
  // Also assuming these boundaries are inclusive.
  if (this->value < this->soft_max) {
    this->value++;
  }

  return this->value;
}

int Counterator::decrement(){
  // Bound the counter by soft limits. We don't have to check the hard limits
  // since they should already be within bounds. We don't need to check the
  // minimums, either, since INTEGER UNDERFLOW CANNOT POSSIBLY HAPPEN. >_>
  // Also assuming these boundaries are inclusive.
  if (this->value > this->soft_min) {
    this->value--;
  }

  return this->value;
}

int Counterator::incSoftMax() {
  return this->setSoftMax(this->getSoftMax() + 1);
}

int Counterator::decSoftMax() {
  return this->setSoftMax(this->getSoftMax() - 1);
}

int Counterator::reset() {
  // Reset counter to current soft_maximum (since we're a decrementer at heart)
  return this->setValue(this->soft_max);
}
