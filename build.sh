#!/bin/sh
gcc -w -o pentaseq main.c synth.c melody.c paUtils.c \
	-I/usr/local/include \
	-L/usr/local/lib -lportaudio -lncurses
