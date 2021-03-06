/*
 * rapidshark_mk_iv.h
 *
 * Arduino microcontroller code to do All The Things to make darts fly. Various
 * defines, consts, enums and other programming wankery.
 *
 * Author: Sean "schizobovine" Caulfield <sean@yak.net>
 * License: GPLv2 (firmware) / CC4.0-BY-SA (documentation, hardware)
 *
 */

#ifndef __RAPIDSHARK_MK_IV_h
#define __RAPIDSHARK_MK_IV_h

////////////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////////////

//
// Pin assignments
//


#define PIN_SW_PUSH     2
#define PIN_ACCEL_PWM   3
#define PIN_SW_CLIP     4
#define PIN_SW_ACCEL    5
#define PIN_SW_FIRE     6
#define PIN_PUSH_A      7
#define PIN_TACHOMETER  8
#define PIN_PUSH_B      9
#define PIN_PUSH_PWM    11
#define PIN_BUTT        13
#define PIN_DISP_RST    -1

//
// Display parameters
//

#define DISP_TEXT_SMALL 1
#define DISP_TEXT_LARGE 2
#define DISP_COLOR      WHITE
#define DISP_ADDR       0x3C
#define DISP_MODE       SSD1306_SWITCHCAPVCC

//
// Motor speed settings
//

#define MOTOR_ACCEL_SPEED     128
#define MOTOR_ACCEL_SPEED_MAX 255
#define MOTOR_PUSH_SPEED      96

// Number of microseconds per rotational period for 20krpm, the measured
// average speed at half-throttle, so we can full-throttle accelerate to that
// speed.
//
//                1
// tau = -------------------- * 1000 (ms/sec) * 1000 (us/ms)
//       20krpm / 60(sec/min)
//     = 60 * 1000 * 1000 / 20000 us
//     = 3000 us

#define MOTOR_ACCEL_SET_PERIOD (3000)

//
// Debounce intervals (in ms)
//

#define DEBOUNCE_TACHOMETER   (1)
#define DEBOUNCE_PUSH         (1)
#define DEBOUNCE_CLIP         (10)
#define DEBOUNCE_FIRE         (10)
#define DEBOUNCE_ACCEL        (10)
#define DEBOUNCE_BUTT         (10)

//
// Helper macros for readability
//
#define IS_PUSHER_EXTENDED  (switchPusher.read()==HIGH)
#define IS_PUSHER_RETRACTED (switchPusher.read()==LOW)
#define IS_ACC_TRIG_OPEN    (switchAccelTrigger.read()==HIGH)
#define IS_ACC_TRIG_CLOSED  (switchAccelTrigger.read()==LOW)
#define IS_FIRE_TRIG_OPEN   (switchFireTrigger.read()==HIGH)
#define IS_FIRE_TRIG_CLOSED (switchFireTrigger.read()==LOW)
#define IS_CLIP_INSERTED    (switchClipDetect.read()==HIGH)

////////////////////////////////////////////////////////////////////////
// PROTOTYPES
////////////////////////////////////////////////////////////////////////

bool finishedAccel();
void setMotorState();

void irq_tach_sens();
void irq_sw_push();
void irq_sw_clip();
void irq_sw_fire();
void irq_sw_accel();
void irq_butt_x();
void irq_butt_y();
void irq_butt_z();

void displayDecimal(uint8_t x, uint8_t y, int num);
void displayLabel(uint8_t x, uint8_t y, const char *text, bool invert);
void displayTextNormal();
void displayTextFlipped();
void displayRefresh();
void displayInit();
void displayDebugData();

void setup();
void loop();

#endif
