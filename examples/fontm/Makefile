#  ____     ___ |    / _____ _____
# |  __    |    |___/    |     |
# |___| ___|    |    \ __|__   |     gsKit Open Source Project.
# ----------------------------------------------------------------------
# Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
# Licenced under Academic Free License version 2.0
# Review gsKit README & LICENSE files for further details.
#
# examples/fontm/Makefile - Makefile for "fontm" example.
#

EE_BIN  = fontm.elf
EE_OBJS = fontm.o

all: $(EE_BIN)

test: all
	ps2client -h $(PS2_IP) -t 1 execee host:fontm.elf

reset: clean
	ps2client -h $(PS2_IP) reset

clean:
	rm -f $(EE_OBJS) $(EE_BIN)

install:

include ../../Makefile.pref
include ../../Makefile.global
