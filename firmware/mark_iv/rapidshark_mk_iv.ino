/*
 * rapidshark_mk_iv.ino
 *
 * Arduino microcontroller code to do All The Things to make darts fly.
 *
 * Author: Sean "schizobovine" Caulfield <sean@yak.net>
 * License: GPLv2 (firmware) / CC4.0-BY-SA (documentation, hardware)
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/power.h>
#include <avr/sleep.h>

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EnableInterrupt.h>
#include <Bounce2.h>

#include "vnh5019.h"
#include "firemode.h"
#include "clip.h"
#include "tach.h"
#include "rapidshark_mk_iv.h"

#define SERIAL_DEBUG 1
#define SERIAL_BAUD_RATE 9600

#if SERIAL_DEBUG
#define DEBUG_PRINTLN(msg) Serial.println((msg))
#define DEBUG_PRINT(msg) Serial.print((msg))
#else
#define DEBUG_PRINTLN(msg)
#define DEBUG_PRINT(msg)
#endif

////////////////////////////////////////////////////////////////////////
// GLOBAL STATE VARIABLES
////////////////////////////////////////////////////////////////////////

// Display controller
Adafruit_SSD1306 display(PIN_DISP_RST);

// Motor controllers
VNH5019 motor_accel = VNH5019(PIN_ACCEL_A, PIN_ACCEL_B, PIN_ACCEL_PWM, MOTOR_ACCEL_SPEED);
VNH5019 motor_push  = VNH5019(PIN_PUSH_A,  PIN_PUSH_B,  PIN_PUSH_PWM,  MOTOR_PUSH_SPEED);

// Button/switch debouncers
Bounce dartDetector;
Bounce switchPusher;
Bounce switchClipDetect;
Bounce switchFireTrigger;
Bounce switchAccelTrigger;
Bounce buttonX;
Bounce buttonY;
Bounce buttonZ;

// Current & total ammo counters
AmmoClip ammo_clip(CLIP_DEFAULT);

// Current fire control mode
FireMode fireMode(MODE_FULL_AUTO);

// Shots left to fire in burst mode
volatile uint8_t burstCounter = 0;

////////////////////////////////////////////////////////////////////////
// MOTOR STATE MACHINE
////////////////////////////////////////////////////////////////////////

/*
 * setMotorState() 
 *
 * Called to figure out if motors should freewheel, brake or go, all based on
 * current system state (is the pusher switch open, fire/accel triggers, and
 * finally fire control mode all factor in). In the stock Rapid Strike, this is
 * handled via eletromechanics such that the three switches comprise the entire
 * state machine. We have to simulate that here.
 *
 */
void setMotorState() {
  
  /*********************************************************************
   *                               PUSHER                              *
   *********************************************************************/

  // If pusher switch is open, this means it's extended outward and needs to be
  // retracted no matter what;
  if (IS_PUSHER_EXTENDED) {

    motor_push.go();

  // If the fire trigger is open, user has let go of it, and so yank brake to
  // stop pusher and thus stop firing.
  } else if (IS_FIRE_TRIG_OPEN) {

    motor_push.brake();

  // Trigger held down, so activate depending on fire control mode and
  // potentially burst counter.
  } else {

    // Keep plugging away for full auto mode; for burst/semi, only activate if
    // we have shots remaining
    if (fireMode.getMode() == MODE_FULL_AUTO || burstCounter > 0) {

      motor_push.go();

    // Otherwise brake motor as we're done
    } else {
      motor_push.brake();
    }

  }
  
  /*********************************************************************
   *                            FLYWHEELS                              *
   *********************************************************************/

  // If the acceleration trigger is being pushed, just go
  if (IS_ACC_TRIG_CLOSED) {
    motor_accel.go();

  // If the fire trigger is being pushed, just go IFF we're supposed to be firing
  } else if (IS_FIRE_TRIG_CLOSED) {

    // Keep plugging away for full auto mode
    // For burst/semi, only activate if we have shots remaining
    if (fireMode.getMode() == MODE_FULL_AUTO || burstCounter > 0) {
      motor_accel.go();

    // Otherwise deactivate
    } else {
      motor_accel.brake();
    }

  // No triggers, no spinny
  } else {
    motor_accel.brake();
  }

}

////////////////////////////////////////////////////////////////////////
// INTERRUPT HANDLERS
////////////////////////////////////////////////////////////////////////

/*
 * irq_dart_detect - Called when the IR sensor gets occluded by a dart.
 */
void irq_dart_detect() {
  if (dartDetector.update() && dartDetector.fell()) {
    ammo_clip.decrement();
  }
}

/*
 * irq_sw_push - Called when the pusher switch opens/closes
 */
void irq_sw_push() {

  // Read switch value and if and only if it's changed, change some state
  if (switchPusher.update()) {

    // If the pusher limit switch is closing, the arm is almost full retracted,
    // meaning we've just completed a single fire cycle. In limited fire modes,
    // this means we should mark off one shot taken.
    if (switchPusher.fell()) {
      if ((fireMode.getMode() == MODE_SEMI_AUTO || fireMode.getMode() == MODE_BURST) && burstCounter > 0) {
        burstCounter--;
      }
    }

    // Change motor states if needed
    setMotorState();

  }
}

/*
 * irq_sw_clip - Called when the clip insert detection switch changed
 */
void irq_sw_clip() {
  if (switchClipDetect.update() && switchClipDetect.fell()) {
    ammo_clip.reset();
  }
}

/*
 * irq_sw_fire - Called when the fire trigger is pulled/released
 */
void irq_sw_fire() {

  // Read switch value and if and only if it's changed, change some state
  if (switchFireTrigger.update()) {

    // On falling signal (user is just starting to close the switch, so pulling
    // the trigger to fire), set the burst counter if we're in that mode.
    if (switchFireTrigger.fell() && burstCounter <= 0) {
      if (fireMode.getMode() == MODE_SEMI_AUTO) {
        burstCounter = 1;
      } else if (fireMode.getMode() == MODE_BURST) {
        burstCounter = 3;
      }

    // On the rising signal (user is letting go of the trigger and it's just
    // opening again), reset burst counter to zero.
    } else if (switchFireTrigger.rose() && burstCounter > 0) {
      burstCounter = 0;
    }

    // Change motor states if needed
    setMotorState();
  }
}

/*
 * irq_sw_accel - Called when the acceleration trigger is pulled/released
 */
void irq_sw_accel() {
  if (switchAccelTrigger.update()) {
    setMotorState();
  }
}

/*
 * irq_butt_x - Called when user presses the X button (down only)
 */
void irq_butt_x() {
  buttonX.update();
}

/*
 * irq_butt_y - Called when user presses the Y button (down only)
 */
void irq_butt_y() {
  buttonY.update();
}

/*
 * irq_butt_z - Called when user presses the Z button (down only)
 */
void irq_butt_z() {
  if (buttonZ.update()) {
    if (buttonZ.rose()) {
      fireMode.nextMode();
    }
  }
}

////////////////////////////////////////////////////////////////////////
// STARTUP CODE
////////////////////////////////////////////////////////////////////////

/*
 * setup() - Main entry point
 */
void setup() {

  // Include serial debugging info if compiled in
#if SERIAL_DEBUG
  delay(500);
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println("HAI");
#endif

  // Setup interrupt handlers
  enableInterrupt(PIN_DART_DETECT,  irq_dart_detect, FALLING);
  enableInterrupt(PIN_SW_PUSH,      irq_sw_push,     CHANGE);
  enableInterrupt(PIN_SW_CLIP,      irq_sw_clip,     FALLING);
  enableInterrupt(PIN_SW_FIRE,      irq_sw_fire,     CHANGE);
  enableInterrupt(PIN_SW_ACCEL,     irq_sw_accel,    CHANGE);
  enableInterrupt(PIN_BUTT_X,       irq_butt_x,      CHANGE);
  enableInterrupt(PIN_BUTT_Y,       irq_butt_y,      CHANGE);
  enableInterrupt(PIN_BUTT_Z,       irq_butt_z,      CHANGE);

  // Setup debouncing objects
  dartDetector      .attach(PIN_DART_DETECT, INPUT_PULLUP, DEBOUNCE_DART_DETECT);
  switchPusher      .attach(PIN_SW_PUSH,     INPUT_PULLUP, DEBOUNCE_PUSH);
  switchClipDetect  .attach(PIN_SW_CLIP,     INPUT_PULLUP, DEBOUNCE_CLIP);
  switchFireTrigger .attach(PIN_SW_FIRE,     INPUT_PULLUP, DEBOUNCE_FIRE);
  switchAccelTrigger.attach(PIN_SW_ACCEL,    INPUT_PULLUP, DEBOUNCE_ACCEL);
  buttonX           .attach(PIN_BUTT_X,      INPUT_PULLUP, DEBOUNCE_BUTT_X);
  buttonY           .attach(PIN_BUTT_Y,      INPUT_PULLUP, DEBOUNCE_BUTT_Y);
  buttonZ           .attach(PIN_BUTT_Z,      INPUT_PULLUP, DEBOUNCE_BUTT_Z);

  // Set motors to know good state (i.e., not running)
  motor_accel.init();
  motor_push.init();

  // Boot up display and show "splash" screen
  displayInit();
  delay(500);

}

////////////////////////////////////////////////////////////////////////
// MAIN LOOP
////////////////////////////////////////////////////////////////////////

void loop() {

  // Change motor states if needed
  setMotorState();

  // Update display
  displayRefresh();

  // Put CPU to sleep until an event (likely one of our timer or pin change
  // interrupts) wakes it
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_mode();

}
