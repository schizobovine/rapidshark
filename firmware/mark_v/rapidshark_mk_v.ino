/*
 * rapidshark_mark_v.ino
 *
 * Arduino microcontroller code to do All The Things to make darts fly.
 *
 * Author: Sean "schizobovine" Caulfield <sean@yak.net>
 * License: GPLv2 (firmware) / CC4.0-BY-SA (documentation, hardware)
 *
 */

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Bounce2.h>
#include <SPI.h>
#include <Wire.h>
//#include <EnableInterrupt.h>
#include <FreqMeasure.h>

////////////////////////////////////////////////////////////////////////
// PIN CONFIGURATION & COMPILE TIME SETTINGS
////////////////////////////////////////////////////////////////////////

#define PIN_SW_PUSH     2
#define PIN_ACCL_PWM    3
#define PIN_SW_CLIP     4
#define PIN_SW_ACCL     5
#define PIN_SW_FIRE     6
#define PIN_PUSH_A      7
#define PIN_TACHOMETER  8
#define PIN_PUSH_B      9
#define PIN_PUSH_PWM    11
#define PIN_SW_BUTT     13
#define PIN_DISP_RST    -1

#define DEBOUNCE_PUSH       (10)
#define DEBOUNCE_CLIP       (10)
#define DEBOUNCE_FIRE       (50)
#define DEBOUNCE_ACCL       (50)
#define DEBOUNCE_BUTT       (50)

#define DISP_ADDR       0x3C
#define DISP_MODE       SSD1306_SWITCHCAPVCC
#define DISP_TEXT_SMALL 1
#define DISP_TEXT_LARGE 2
#define DISP_COLOR      WHITE

//
// Helper macros for readability
//
#define IS_PUSH_EXTENDED  (switchPush.read()==HIGH)
//#define IS_PUSH_RETRACTED (switchPush.read()==LOW)
//#define IS_ACCL_OPEN      (switchAccl.read()==HIGH)
#define IS_ACCL_CLOSED    (switchAccl.read()==LOW)
#define IS_FIRE_OPEN      (switchFire.read()==HIGH)
#define IS_FIRE_CLOSED    (switchFire.read()==LOW)
#define IS_CLIP_INSERTED  (switchClip.read()==HIGH)
#define IS_FULL_AUTO      (fireMode == FIREMODE_FULLAUTO)
#define KEEP_FIRING       (IS_FULL_AUTO || burstCount > 0)

#define FIREMODE_SINGLE   0
#define FIREMODE_BURST    1
#define FIREMODE_FULLAUTO 2

#define BURST_MODE_ROUNDS 3

// Pusher motor PWM settings
#define PUSH_PWM_GO    96
#define PUSH_PWM_BRAKE 255

// Accel motor PWM setting(s)
#define ACCL_PWM_GO    128
#define ACCL_PWM_BRAKE 0

////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
////////////////////////////////////////////////////////////////////////

// Display controller
Adafruit_SSD1306 display(PIN_DISP_RST);

// Button/switch debouncers
Bounce switchPush;
Bounce switchClip;
Bounce switchFire;
Bounce switchAccl;
Bounce switchButt;

// Ammo counter
uint8_t dartCount = 37;

// Burst shot counter
uint8_t burstCount = BURST_MODE_ROUNDS;

// Current fire mode
uint8_t fireMode = FIREMODE_SINGLE;

////////////////////////////////////////////////////////////////////////
// MOTOR STATE MACHINE
////////////////////////////////////////////////////////////////////////

// Set pusher motor state based on current conditions (are we firing? are there
// burst rounds left to fire? etc.).
void setPushState() {

  // If pusher switch is open, this means it's extended outward and needs to be
  // retracted no matter what; or if we have rounds left to fire for the
  // current mode.
  if (IS_PUSH_EXTENDED) {
    pusher_go();
  }

  // If the fire trigger is open, user has let go of it, and so yank brake to
  // stop pusher and thus stop firing.
  else if (IS_FIRE_OPEN) {
    pusher_brake();

  // Trigger held down, so activate depending on fire control mode and
  // potentially burst counter: keep plugging away for full auto mode; for
  // burst/semi, only activate if we have shots remaining
  } else if (KEEP_FIRING) {
    pusher_go();
  }

  // Otherwise brake motor
  else {
    pusher_brake();
  }
  
}

// Set flywheel motor state based on current conditions
void setAcclState() {

  // If the acceleration trigger is being pushed OR if the fire trigger is
  // being pushed & we're supposed to be firing, run motors
  if (IS_ACCL_CLOSED || ((IS_FIRE_CLOSED) && (KEEP_FIRING))) {
    analogWrite(PIN_ACCL_PWM, ACCL_PWM_GO);
    FreqMeasure.begin();
  }

  // Otherwise, brake motor & reset tach
  else {
    analogWrite(PIN_ACCL_PWM, ACCL_PWM_BRAKE);
    FreqMeasure.end();
  }

}

////////////////////////////////////////////////////////////////////////
// INTERRUPT HANDLERS
////////////////////////////////////////////////////////////////////////

void irq_sw_push() {
  if (switchPush.update()) {
    // Update motor state
  }
}

void irq_sw_fire() {
  if (switchFire.update()) {
    // Update motor state
  }
}

void irq_sw_accl() {
  if (switchAccl.update()) {
    // Update motor state
  }
}

void irq_sw_clip() {
  if (switchClip.update()) {
    // Update ammo counter state
  }
}

void irq_sw_butt() {
  if (switchButt.update()) {
    // Update fire mode
  }
}

////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
////////////////////////////////////////////////////////////////////////

// Engage pusher motor to start shoving out darts!
void pusher_go() {
  digitalWrite(PIN_PUSH_A, HIGH);
  digitalWrite(PIN_PUSH_B, LOW);
  analogWrite(PIN_PUSH_PWM, PUSH_PWM_GO);
}

// Actively brake pusher motor to keep the pusher from being left sticking out.
void pusher_brake() {
  digitalWrite(PIN_PUSH_A, LOW);
  digitalWrite(PIN_PUSH_B, LOW);
  analogWrite(PIN_PUSH_PWM, PUSH_PWM_BRAKE);
}

////////////////////////////////////////////////////////////////////////
// SETUP
////////////////////////////////////////////////////////////////////////

void setup() {

  // Initialize debouncers & set pin mode
  switchPush.attach(PIN_SW_PUSH, INPUT_PULLUP, DEBOUNCE_PUSH);
  switchClip.attach(PIN_SW_CLIP, INPUT_PULLUP, DEBOUNCE_CLIP);
  switchFire.attach(PIN_SW_FIRE, INPUT_PULLUP, DEBOUNCE_FIRE);
  switchAccl.attach(PIN_SW_ACCL, INPUT_PULLUP, DEBOUNCE_ACCL);
  switchAccl.attach(PIN_SW_BUTT, INPUT_PULLUP, DEBOUNCE_BUTT);

  // Register interrupt handlers which invoke debouncers
  //attachInterrupt(PIN_SW_PUSH, irq_sw_push, CHANGE);
  //attachInterrupt(PIN_SW_CLIP, irq_sw_clip, CHANGE);
  //attachInterrupt(PIN_SW_FIRE, irq_sw_fire, CHANGE);
  //attachInterrupt(PIN_SW_ACCL, irq_sw_accl, CHANGE);
  //attachInterrupt(PIN_SW_BUTT, irq_sw_butt, FALLING);
  
  // Setup i2c & display
  Wire.begin();
  display.begin(DISP_MODE, DISP_ADDR);
  display.clearDisplay();
  display.setTextSize(DISP_TEXT_LARGE);
  display.setTextWrap(false);
  display.println();
  display.print("RapidShark");
  display.print("  Mark IV");
  display.dim(false);
  display.display();

}

////////////////////////////////////////////////////////////////////////
// MAIN LOOP
////////////////////////////////////////////////////////////////////////

void loop() {
}
