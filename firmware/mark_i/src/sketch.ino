/*
 * locknload.ino - Nerf gun controller code.
 *
 * This particular display is common cathode, meaning that which digit the
 * circuit is driving is set by grounding the appropiate digit's pin (either
 * pin 1 or 10 in the diagram below). The other pins are the anodes for the
 * positions within each digit. The only way I can figure out how to get
 * different digits displayed using this is PWM. :P
 *
 *  1  2  3  4  5               B       
 *  \  |  |  |  /             -----
 * +-------------+           |     |
 * |   ---   --- |        A->|     |<-C
 * |  |  |  |  | |           |  G  |
 * |  ---   ---  |            -----
 * | |  |  |  |  |           |     |
 * | --- * --- * |        F->|     |<-D
 * +-------------+           |     |
 *  /  |  |  |  \             ----- *<-P
 *  6  7  8  9  10              E
 *
 */

#include <Arduino.h>
#include <limits.h>
#include "button.h"
#include "counter.h"
#include "display.h"
#include "usec.h"

#define SERIAL_DEBUG 0

#if SERIAL_DEBUG
#define DEBUG_PRINTLN(msg) Serial.println((msg))
#define DEBUG_PRINT(msg) Serial.print((msg))
#else
#define DEBUG_PRINTLN(msg)
#define DEBUG_PRINT(msg)
#endif

////////////////////////////////////////////////////////////////////////
// PIN ASSIGNMENT CONSTANTS
////////////////////////////////////////////////////////////////////////

// Mapping by Arduino pin
//
//  2 : A      7 : F
//  3 : B      5 : D
// NC : P      4 : C
//  8 : G     10 : MSD cathode
//  6 : E      9 : LSD cathode

// Mapping by display position
const int LED_VCC_A = 13;
const int LED_VCC_B = 12;
const int LED_VCC_C = 6;
const int LED_VCC_D = 7;
const int LED_VCC_E = 9;
const int LED_VCC_F = 8;
const int LED_VCC_G = 10;
const int LED_VCC_P = -1; //not connected
const int LED_GND_LSD = A0;
const int LED_GND_MSD = A1;

// Status indicator for missing clip (solid) and RELOAD, BITCH (blinking)
const int YELLOW_LED = A3;

// Switch that is tripped while a clip is in the weapon
const int CLIP_DETECT = 11;

// Buttons for controlling the counter
const int BUTT_START = 2;
const int BUTT_PREV = 4; //3
const int BUTT_NEXT = 5; //4
const int BUTT_STOP = 3; //5

// Detects if a dart has been fired
const int PUSHER_DETECT = A2;

////////////////////////////////////////////////////////////////////////
// OTHER CONSTANTS
////////////////////////////////////////////////////////////////////////

// Serial speed, set to the most retarded but hopefully compatibile default.
const int SERIAL_BAUD_RATE = 9600;

// Bounds for counter. Soft min/max are based on the capacity of the current
// clip; the hard min/max are based on the display's capabilities. Default
// value is for the clip I normally use (35-round drum that actually holds 37
// without squishing).
const int COUNTER_SOFT_MIN = 0;
const int COUNTER_SOFT_MAX = 37;
const int COUNTER_HARD_MIN = 0;
const int COUNTER_HARD_MAX = 99;
const int COUNTER_DEFAULT = 37;

// Minimum amount of time (in microseconds) that must pass before we will allow
// the button states to toggle. Helps prevent bouncing.
const unsigned long MIN_TRIP_INTERVAL = 100;

// Duty cycle length for the LED display PWM states (in microseconds).
const unsigned long DISPLAY_PWM_OFF = 1000;
const unsigned long DISPLAY_PWM_MSD = 1000;
const unsigned long DISPLAY_PWM_CLR = 1000;
const unsigned long DISPLAY_PWM_LSD = 1000;

// Flash interval when clip is empty (microseconds)
const unsigned long BLINK_RATE = 500000; // 0.5s

////////////////////////////////////////////////////////////////////////
// GLOBAL STATE VARIABLES
////////////////////////////////////////////////////////////////////////

// Buttons for controlling the counter's value and soft limits
Button *buttStart;
Button *buttPrev;
Button *buttNext;
//Button *buttStop;

// Counter controller
Counterator *counter;

// Which to display, current count or current max (for setting it)?
bool displaySoftMaxCount = false;

// Display controller
Display *display;

// LED control
bool blink_state = false;
usec last_blink = 0;
bool clip_was_present = false;

// Reed switch to detect when darts leave the barrel for ammo counter
bool trip_state = false;

////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
////////////////////////////////////////////////////////////////////////

// Handle user input via the control puttons
void handleUserFrobulation() {

  // Check and save all values first for later use (so no race conditions or
  // other hard to debug weirdness).
  bool pushed_start = buttStart->check();
  bool pushed_next = buttNext->check();
  bool pushed_prev = buttPrev->check();
  bool pushed_stop = (digitalRead(BUTT_STOP) == LOW);

  // Simple reset button in case the clip drop one fails in the field
  if (pushed_start) {
    DEBUG_PRINTLN("BUTT START");
    counter->reset();
  }

  // Mode to set the soft maximum, i.e., clip count
  if (pushed_stop) {
    displaySoftMaxCount = true;

    // Decrement ammo count, dropping soft limit with us only if we're at the
    // soft max
    if (pushed_prev) {
      DEBUG_PRINT("BUTT PREV (MAX MODE)\r\nsoft_max = " + String(counter->getSoftMax()) + " (before), ");
      int after = counter->decSoftMax();
      DEBUG_PRINTLN(String(after) + " (after)");
    }

    // Increment ammo count, bumping up soft limit if we hit it
    if (pushed_next) {
      DEBUG_PRINT(" BUTT NEXT (MAX MODE)\r\nsoft_max = " + String(counter->getSoftMax()) + " (before), ");
      int after = counter->incSoftMax();
      DEBUG_PRINTLN(String(after) + " (after)");
    }

  // Check other two buttons exclusive of the above. These will always obey the
  // soft limits.
  } else {
    displaySoftMaxCount = false;

    if (pushed_prev) {
      counter->decrement();
      DEBUG_PRINTLN("BUTT PREV (count="+String(counter->getValue()+")"));
    }

    if (pushed_next) {
      counter->increment();
      DEBUG_PRINTLN("BUTT NEXT (count="+String(counter->getValue()+")"));
    }

  }

}

// Clip && low ammo detection which controls the yellow blinker.
void handleYellowLED() {
  bool clip_present = (digitalRead(CLIP_DETECT) == LOW);

  // Light up the clip missing LED if it's not present; LED 
  if (!clip_present) {
    digitalWrite(YELLOW_LED, HIGH);
    clip_was_present = false;

  // If the clip was just re-inserted, turn off LED & reset counter
  } else if (clip_present && !clip_was_present) {
    digitalWrite(YELLOW_LED, LOW);
    DEBUG_PRINTLN("LOCK AND LOAD, BITCHES");
    counter->reset();

  // Check ammo counter & flash LED if it's at 0
  } else if (counter->getValue() == 0) {
    usec now = micros();
    if (USEC_DIFF(now, last_blink) >= BLINK_RATE) {
      last_blink = now;
      blink_state = !blink_state;
    }
    digitalWrite(YELLOW_LED, (blink_state ? HIGH : LOW));

  // Otherwise turn LED off
  } else {
    digitalWrite(YELLOW_LED, LOW);
  }

  // Store for next loop
  clip_was_present = clip_present;

}

// Dart firing code: check if reed switch on top of the pusher motor is closed
// (indicating the pusher arm is retracted because the embedded magnet is
// closing the reed switch) or open (arm is extended).
void handlePusherDetect() {
  bool value;
  
  if (digitalRead(PUSHER_DETECT) == LOW) {
    trip_state = true;
  } else {
    if (trip_state) {
      trip_state = false;
      DEBUG_PRINTLN("BANG");
      counter->decrement();
    }
  }

}

////////////////////////////////////////////////////////////////////////
// STARTUP CODE
////////////////////////////////////////////////////////////////////////

void setup() {

#if SERIAL_DEBUG
  delay(200);
  // Setup serial output for debugging
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println("HAI");
#endif

  pinMode(YELLOW_LED, OUTPUT);
  pinMode(CLIP_DETECT, INPUT_PULLUP);
  pinMode(PUSHER_DETECT, INPUT_PULLUP);

  pinMode(BUTT_START, INPUT_PULLUP);
  pinMode(BUTT_PREV, INPUT_PULLUP);
  pinMode(BUTT_NEXT, INPUT_PULLUP);
  pinMode(BUTT_STOP, INPUT_PULLUP);

  display = new Display(
    LED_VCC_A,
    LED_VCC_B,
    LED_VCC_C,
    LED_VCC_D,
    LED_VCC_E,
    LED_VCC_F,
    LED_VCC_G,
    LED_VCC_P,
    LED_GND_MSD,
    LED_GND_LSD,
    DISPLAY_PWM_OFF,
    DISPLAY_PWM_MSD,
    DISPLAY_PWM_CLR,
    DISPLAY_PWM_LSD
  );

  buttStart = new Button(BUTT_START, true, MIN_TRIP_INTERVAL);
  buttPrev = new Button(BUTT_PREV, true, MIN_TRIP_INTERVAL);
  buttNext = new Button(BUTT_NEXT, true, MIN_TRIP_INTERVAL);
  //buttStop = new Button(BUTT_STOP, true, MIN_TRIP_INTERVAL);

  counter = new Counterator(
    COUNTER_SOFT_MIN,
    COUNTER_SOFT_MAX,
    COUNTER_HARD_MIN,
    COUNTER_HARD_MAX,
    COUNTER_DEFAULT
  );

}

////////////////////////////////////////////////////////////////////////
// MAIN LOOP
////////////////////////////////////////////////////////////////////////

void loop() {

  // Handle user input
  handleUserFrobulation();

  // Check if a dart has been fired
  handlePusherDetect();

  // Shove all the handling for this one goddamn blinker into a function
  handleYellowLED();

  // Write counter value to display, which will also have the side effect of
  // refreshing the display.
  if (displaySoftMaxCount) {
    display->put(counter->getSoftMax());
  } else {
    display->put(counter->getValue());
  }

}
