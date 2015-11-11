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
VNH5019 motor_accel = VNH5019(PIN_ACCEL_A, PIN_ACCEL_B, PIN_ACCEL_PWM);
VNH5019 motor_push  = VNH5019(PIN_PUSH_A, PIN_PUSH_B, PIN_PUSH_PWM);

// Button/switch debouncers
Bounce dartDetector;
Bounce switchPusher;
Bounce switchClipDetect;
Bounce switchFireTrigger;
Bounce switchAccelTrigger;
Bounce buttonX;
Bounce buttonY;
Bounce buttonZ;

// Important state variables
volatile bool isPusherSwitchOpen = false;
volatile bool isClipDetected = false;
volatile bool trigFire = false;
volatile bool trigAccel = false;

// Current & total ammo counters
volatile uint8_t ammoCounter = 0;
uint8_t ammoCounterTotal = 0;

// Current fire control mode
fire_mode_t fireMode = MODE_FULL_AUTO;

// Shots left to fire in burst mode
volatile uint8_t burstCounter = 0;

////////////////////////////////////////////////////////////////////////
// "HALPING" FUNCTIONS
////////////////////////////////////////////////////////////////////////

/*
 * refreshDisplay
 *
 * Update status information on the display.
 *
 */
void refreshDisplay() {

  display.clearDisplay();

  displayTextNormal();
  display.setTextSize(4);
  display.setCursor(40, 0);
  display.print("37");

  display.setTextSize(1);

  displayLabel( 0, 40, "ACC" , (trigAccel));
  displayLabel( 0, 48, "FIRE", (trigFire));
  displayLabel( 0, 56, "PUSH", (IS_PUSHER_EXTENDED));
  displayLabel(30, 40, "DART", (dartDetector.read()));
  displayLabel(30, 48, "CLIP", (isClipDetected));

  display.display();

}

void displayLabel(uint8_t x, uint8_t y, const char *text, bool invert) {
  display.setCursor(x, y);
  if (invert) {
    displayTextFlipped();
  } else {
    displayTextNormal();
  }
  display.print(text);
  displayTextNormal();
}

void displayTextNormal() {
  display.setTextColor(WHITE);
}

void displayTextFlipped() {
  display.setTextColor(BLACK, WHITE);
}

////////////////////////////////////////////////////////////////////////
// MOTOR STATE MACHINE
////////////////////////////////////////////////////////////////////////

/*
 * setMotorState() 
 *
 * Called to figure out if motors should freewheel, brake or go, all based on
 * current system state (isPusherSwitchOpen, fire/accel triggers, and finally
 * fire control mode all factor in). In the stock Rapid Strike, this is handled
 * via eletromechanics such that the three switches comprise the entire state
 * machine. We have to simulate that here.
 *
 */
void setMotorState() {
 setPusherMotorState();
 setAccelMotorState();
}

/*
 * setPusherMotorState() 
 *
 * Called to figure out if pusher should brake or go, based on current system
 * state (isPusherSwitchOpen, fire/accel triggers, and finally fire control
 * mode all factor in).
 *
 */
void setPusherMotorState() {
  
  // If pusher switch is open, this means it's extended outward and needs to be
  // retracted no matter what;
  if (IS_PUSHER_EXTENDED) {
    motor_push.go();
  }

  else {
    motor_push.brake_gnd();
  }

}

/*
 * setAccelMotorState() 
 *
 * Called to figure out if flywheel motors should freewheel or go, based on
 * current system state (fire/accel triggers and fire control mode all factor
 * in).
 *
 */
void setAccelMotorState() {
  
  motor_accel.brake_gnd();

}

////////////////////////////////////////////////////////////////////////
// INTERRUPT HANDLERS
////////////////////////////////////////////////////////////////////////

/*
 * irq_dart_detect - Called when the IR sensor gets occluded by a dart.
 */
void irq_dart_detect() {
  if (dartDetector.update()) {
    if (dartDetector.fell() && ammoCounter > 0) {
      ammoCounter--;
    }
  }
}

/*
 * irq_sw_push - Called when the pusher switch opens/closes
 */
void irq_sw_push() {
  if (switchPusher.update()) {
    isPusherSwitchOpen = (switchPusher.read() == HIGH);
    setMotorState();
  }
}

/*
 * irq_sw_clip - Called when the clip insert detection switch changed
 */
void irq_sw_clip() {
  if (switchClipDetect.update()) {
    isClipDetected = (switchClipDetect.read() == HIGH);
  }
}

/*
 * irq_sw_fire - Called when the fire trigger is pulled/released
 */
void irq_sw_fire() {
  if (switchFireTrigger.update()) {
    trigFire = (switchFireTrigger.read());
    setMotorState();
  }
}

/*
 * irq_sw_accel - Called when the acceleration trigger is pulled/released
 */
void irq_sw_accel() {
  if (switchAccelTrigger.update()) {
    trigAccel = (switchAccelTrigger.read());
    setMotorState();
  }
}

/*
 * irq_butt_x - Called when user presses the X button (down only)
 */
void irq_butt_x() {
  if (buttonX.update()) {
  }
}

/*
 * irq_butt_y - Called when user presses the Y button (down only)
 */
void irq_butt_y() {
  if (buttonY.update()) {
  }
}

/*
 * irq_butt_z - Called when user presses the Z button (down only)
 */
void irq_butt_z() {
  if (buttonZ.update()) {
  }
}

////////////////////////////////////////////////////////////////////////
// STARTUP CODE
////////////////////////////////////////////////////////////////////////

/*
 * init_irq - Setup interrupt handling routines
 */
void init_irq() {
  enableInterrupt(PIN_DART_DETECT,  irq_dart_detect, CHANGE);
  enableInterrupt(PIN_SW_PUSH,      irq_sw_push,     CHANGE);
  enableInterrupt(PIN_SW_CLIP,      irq_sw_clip,     CHANGE);
  enableInterrupt(PIN_SW_FIRE,      irq_sw_fire,     CHANGE);
  enableInterrupt(PIN_SW_ACCEL,     irq_sw_accel,    CHANGE);
  enableInterrupt(PIN_BUTT_Z,       irq_butt_x,      CHANGE);
  enableInterrupt(PIN_BUTT_Y,       irq_butt_y,      CHANGE);
  enableInterrupt(PIN_BUTT_X,       irq_butt_z,      CHANGE);
}

/*
 * init_bouncers - Setup debouncing objects
 */
void init_bouncers() {
  dartDetector.attach(PIN_DART_DETECT, INPUT_PULLUP);
  switchPusher.attach(PIN_SW_PUSH, INPUT_PULLUP);
  switchClipDetect.attach(PIN_SW_CLIP, INPUT_PULLUP);
  switchFireTrigger.attach(PIN_SW_FIRE, INPUT_PULLUP);
  switchAccelTrigger.attach(PIN_SW_ACCEL, INPUT_PULLUP);
  buttonX.attach(PIN_BUTT_Z, INPUT_PULLUP);
  buttonY.attach(PIN_BUTT_Y, INPUT_PULLUP);
  buttonZ.attach(PIN_BUTT_X, INPUT_PULLUP);
}

/*
 * init_motors - Configure & set to known state
 */
void init_motors() {
  motor_accel.init();
  motor_accel.setSpeed(MOTOR_ACCEL_SPEED);
  motor_push.init();
  motor_push.setSpeed(MOTOR_PUSH_SPEED_FAST);
}

/*
 * init_display - Boot up display and print out something to show it works
 */
void init_display() {
  Wire.begin();
  display.begin(DISP_MODE, DISP_ADDR);
  display.clearDisplay();
  display.setTextColor(DISP_COLOR);
  display.setTextSize(DISP_TEXT_LARGE);
  display.println();
  display.print("RapidShark");
  display.print("  Mark IV");
  display.dim(false);
  display.setTextWrap(false);
  display.display();
}

/*
 * setup() - Main entry point
 */
void setup() {

#if SERIAL_DEBUG
  delay(500);
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println("HAI");
#endif

  init_motors();
  init_bouncers();
  init_irq();
  init_display();
  //set_sleep_mode(SLEEP_MODE_IDLE);

  delay(500);

}

////////////////////////////////////////////////////////////////////////
// MAIN LOOP
////////////////////////////////////////////////////////////////////////

void loop() {

  // Only bother updating the display if anything's changed
  //if (needsRefresh) {
    //needsRefresh = false;
    refreshDisplay();
  //}

  // Put CPU to sleep until an event (likely one of our timer or pin change
  // interrupts) wakes it
  //sleep_mode();
  delay(100);

}
