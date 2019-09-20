/*
 * counter.h - Simple utility class for keeping track of a counter, including
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

#ifndef COUNTERATOR_H
#define COUNTERATOR_H

#define MIN(x, y) (((x) <= (y)) ? (x) : (y))
#define MAX(x, y) (((x) >= (y)) ? (x) : (y))

class Counterator {
  public:

    /*
     * Shorter version sets soft=hard.
     */
    Counterator(int min, int max, int value);
    Counterator(int soft_min, int soft_max, int hard_min, int hard_max, int value);

    /*
     * Manipulate current soft limits.
     *
     * @returns new limit value, possibly unchanged if you tried to push it
     * past a hard limit.
     */
    int getSoftMin();
    int setSoftMin(int new_soft_min);
    int getSoftMax();
    int setSoftMax(int new_soft_max);

    /*
     * Hard limits are hard.
     *
     * @returns Get the current hard limits.
     */
    int getHardMin();
    int getHardMax();

    /*
     * Get the counter's current value without altering it.
     */
    int getValue();

    /*
     * Set the counter to an explict value (still obeying limits).
     */
    int setValue(int newValue);

    /*
     * Increments/decrements the counter, obeying hard and soft limits.
     *
     * @return The new value of the counter, possibly unchanged if hard/soft
     * limits are hit.
     *
     */
    int increment();
    int decrement();
    int reset();
    int incSoftMax();
    int decSoftMax();

  private:
    int soft_min;
    int soft_max;
    int hard_min;
    int hard_max;
    int value;
};

#endif
