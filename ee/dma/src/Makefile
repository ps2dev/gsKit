#  ____     ___ |    / _____ _____
# |  __    |    |___/    |     |
# |___| ___|    |    \ __|__   |     gsKit Open Source Project.
# ----------------------------------------------------------------------
# Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
# Licenced under Academic Free License version 2.0
# Review gsKit README & LICENSE files for further details.
#
# ee/gs/srcMakefile - gsKit gs makefile.
#

EE_OBJS = 	dmaCore.o \
		dmaInit.o \
		dmaSpr.o

EE_CFLAGS += -Wno-unused

all: $(EE_OBJS)
	$(EE_AR) rcs $(LIBDMAKIT) $(EE_OBJS)

clean:
	rm -f $(EE_OBJS)

include ../../../Makefile.pref
include ../../../Makefile.global

