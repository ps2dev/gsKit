#  ____     ___ |    / _____ _____
# |  __    |    |___/    |     |
# |___| ___|    |    \ __|__   |     gsKit Open Source Project.
# ----------------------------------------------------------------------
# Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
# Licenced under Academic Free License version 2.0
# Review gsKit README & LICENSE files for further details.
#
# Makefile - gsKit root makefile.
#

#ifneq (x$GSKIT), x)
#GSKITSRC=$(GSKIT)
#endif

ifeq (x$(GSKITSRC), x)
GSKITSRC=`pwd`
endif

SUBDIRS = ee examples lib

include Rules.make

reset:
	ps2client reset

