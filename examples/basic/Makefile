#  ____     ___ |    / _____ _____
# |  __    |    |___/    |     |
# |___| ___|    |    \ __|__   |     gsKit Open Source Project.
# ----------------------------------------------------------------------
# Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
# Licenced under Academic Free License version 2.0
# Review gsKit README & LICENSE files for further details.
#
# examples/basic/Makefile - Makefile for "basic" example.
#

EE_BIN  = basic.elf
EE_OBJS = basic.o

all: $(EE_BIN)

test: all
	ps2client -t 1 execee host:basic.elf

reset: clean
	ps2client reset

clean:
	rm -f $(EE_OBJS) $(EE_BIN)


include ../../Makefile.pref
include ../../Makefile.global
