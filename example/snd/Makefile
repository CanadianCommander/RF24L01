ARDUINO_DIR   = /Applications/Arduino.app/Contents/Java
ARDMK_DIR     = /usr/local/Cellar/arduino-mk/1.5.2/
AVR_TOOLS_DIR = /usr/local
MONITOR_PORT  = /dev/cu.wchusbserial14110    #FOR NANO
#MONITOR_PORT = /dev/cu.usbmodem14311
CXXFLAGS += -I ../../
USER_LIB_PATH := ../../../

ARDUINO_LIBS = RF24L01_Library
#BOARD_TAG     = uno
BOARD_TAG    = nano
BOARD_SUB			= atmega328
MCU = atmega328p
F_CPU = 16000000UL

include $(ARDMK_DIR)/Arduino.mk
