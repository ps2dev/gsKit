#  ____     ___ |    / _____ _____
# |  __    |    |___/    |     |
# |___| ___|    |    \ __|__   |     gsKit Open Source Project.
# ----------------------------------------------------------------------
# Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
# Licenced under Academic Free License version 2.0
# Review gsKit README & LICENSE files for further details.
#
# ee/gs/src/Makefile - gsKit gs makefile.
#

EE_OBJS = 	gsCore.o \
		gsFont.o \
		gsInit.o \
		gsMisc.o \
		gsPrimitive.o \
		gsTexture.o \
		gsVU1.o

EE_CFLAGS += -g

all: $(EE_OBJS)
	$(EE_AR) rcs $(LIBGSKIT) $(EE_OBJS)

clean:
	rm -f $(EE_OBJS)

install:

include ../../../Makefile.pref
include ../../../Makefile.global

