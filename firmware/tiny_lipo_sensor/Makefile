ARDUINO_DIR       := $(HOME)/arduino/arduino-current/
ARDUINO_LIBS      := 
#ARDUINO_VAR_PATH  := $(HOME)/.arduino15/packages/attiny/hardware/avr/1.0.2/
BOARDS_TXT        := $(HOME)/.arduino15/packages/attiny/hardware/avr/1.0.2/boards.txt
ALTERNATE_CORE    := attiny
ARDUINO_VAR_PATH  := $(HOME)/.arduino15/packages/attiny/hardware/avr/1.0.2/
ARDUINO_PORT      := /dev/ttyUSB0
USER_LIB_PATH     := $(realpath ./lib)
BOARD_TAG         := attiny
BOARD_SUB         := ATtiny84

CXXFLAGS      += -std=gnu++11 #-Wl,-u,vfprintf
CFLAGS        += -std=gnu++11 #-Wl,-u,vfprintf
#LDFLAGS       += -lprintf_flt -lm -Wl,-u,vfprintf

include Arduino.mk
