#!/bin/sh
avr-gcc -g -Os -mmcu=atmega328p main.c -o a.o # -Wl,-u,vfprintf -lprintf_flt -lm
avr-objcopy -O ihex a.o a.hex
