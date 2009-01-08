#  ____     ___ |    / _____ _____
# |  __    |    |___/    |     |
# |___| ___|    |    \ __|__   |     gsKit Open Source Project.
# ----------------------------------------------------------------------
# Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
# Licenced under Academic Free License version 2.0
# Review gsKit README & LICENSE files for further details.
#
# lib/Makefile - gsKit lib/ Makefile.
#

all:

clean:
	rm -f libgskit.a
	rm -f libdmakit.a
	rm -f libgskit_toolkit.a

install:
	mkdir -p $(GSKIT)/lib
	cp libgskit.a libdmakit.a libgskit_toolkit.a $(GSKIT)/lib

include ../Makefile.global
include ../Makefile.pref
