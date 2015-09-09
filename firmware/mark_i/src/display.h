/*
 * display.h - Helper code for dealing with a 7 segment display.
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include "usec.h"

/*
 *  Digit Layout
 *
 *       B       
 *     -----
 *    |     |
 * A->|     |<-C
 *    |  G  |
 *     -----
 *    |     |
 * F->|     |<-D
 *    |     |
 *     ----- *<-P
 *       E
 */

/* Symbolic masks for each segment */
#define SEG_A (1 << 0)
#define SEG_B (1 << 1)
#define SEG_C (1 << 2)
#define SEG_D (1 << 3)
#define SEG_E (1 << 4)
#define SEG_F (1 << 5)
#define SEG_G (1 << 6)
#define SEG_P (1 << 7)

/* Combine masks to make each digit */
#define DIGIT_0 (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F)
#define DIGIT_1 (SEG_C | SEG_D)
#define DIGIT_2 (SEG_B | SEG_C | SEG_G | SEG_F | SEG_E)
#define DIGIT_3 (SEG_B | SEG_C | SEG_G | SEG_D | SEG_E)
#define DIGIT_4 (SEG_A | SEG_G | SEG_C | SEG_D)
#define DIGIT_5 (SEG_B | SEG_A | SEG_G | SEG_D | SEG_E)
#define DIGIT_6 (SEG_B | SEG_A | SEG_G | SEG_F | SEG_E | SEG_D)
#define DIGIT_7 (SEG_B | SEG_C | SEG_D)
#define DIGIT_8 (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G)
#define DIGIT_9 (SEG_A | SEG_B | SEG_C | SEG_G | SEG_D)

/* Finally, a lookup macro that horribly abuses the ternary operator */
#define LOOKUP_DIGIT_MASK(digit) \
  ((digit) == 0 ? DIGIT_0 : \
  ((digit) == 1 ? DIGIT_1 : \
  ((digit) == 2 ? DIGIT_2 : \
  ((digit) == 3 ? DIGIT_3 : \
  ((digit) == 4 ? DIGIT_4 : \
  ((digit) == 5 ? DIGIT_5 : \
  ((digit) == 6 ? DIGIT_6 : \
  ((digit) == 7 ? DIGIT_7 : \
  ((digit) == 8 ? DIGIT_8 : \
  ((digit) == 9 ? DIGIT_9 : 0))))))))))

#define PWM_STATE_OFF 0
#define PWM_STATE_MSD 1
#define PWM_STATE_CLR 2
#define PWM_STATE_LSD 3

#define DIGITAL_WRITE(pin, value) if ((pin) >= 0) digitalWrite((pin), (value))
#define PINMODE(pin, mode) if ((pin) >= 0) pinMode((pin), (mode))

class Display {

  public:
    Display(
        int seg_a_pin,
        int seg_b_pin,
        int seg_c_pin,
        int seg_d_pin,
        int seg_e_pin,
        int seg_f_pin,
        int seg_g_pin,
        int decimal_pin,
        int msd_pin,
        int lsd_pin,
        usec pwm_usec_off,
        usec pwm_usec_msd,
        usec pwm_usec_clr,
        usec pwm_usec_lsd
    );
    void put(int number);
    void refresh();

  private:
    int seg_a_pin;
    int seg_b_pin;
    int seg_c_pin;
    int seg_d_pin;
    int seg_e_pin;
    int seg_f_pin;
    int seg_g_pin;
    int decimal_pin;
    int msd_pin;
    int lsd_pin;
    int value;
    usec last_flip;
    int pwm_state;
    usec pwm_usec_off;
    usec pwm_usec_msd;
    usec pwm_usec_clr;
    usec pwm_usec_lsd;
    void writeDigit(int digit);
    void clearSegments();
    bool timesUp(usec now, usec interval);
};

#endif
