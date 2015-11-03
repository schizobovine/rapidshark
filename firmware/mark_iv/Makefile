ARDUINO_DIR   := $(HOME)/arduino/arduino-current
ARDUINO_LIBS  := SPI Wire Adafruit_GFX Adafruit_SSD1306 EnableInterrupt
BOARD_TAG     := uno
ARDUINO_PORT  := /dev/ttyUSB1
USER_LIB_PATH := $(realpath ./lib)

CXXFLAGS      += -std=gnu++11 -Wl,-u,vfprintf
CFLAGS        += -std=gnu11 -Wl,-u,vfprintf
LDFLAGS       += -lprintf_flt -lm -Wl,-u,vfprintf

include ./Arduino.mk
