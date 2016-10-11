ARDUINO_DIR       := $(HOME)/arduino/arduino-current/
ARDUINO_LIBS      := Adafruit_GFX Adafruit_SSD1306 SPI Wire Bounce2
ARDUINO_PORT      := /dev/ttyUSB0
USER_LIB_PATH     := $(realpath ./lib)
BOARD_TAG         := uno

CXXFLAGS      += -std=gnu++11 #-Wl,-u,vfprintf
CFLAGS        += -std=gnu++11 #-Wl,-u,vfprintf
#LDFLAGS       += -lprintf_flt -lm -Wl,-u,vfprintf

include Arduino.mk
