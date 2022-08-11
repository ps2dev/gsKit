#  ____     ___ |    / _____ _____
# |  __    |    |___/    |     |
# |___| ___|    |    \ __|__   |     gsKit Open Source Project.
# ----------------------------------------------------------------------
# Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
# Licenced under Academic Free License version 2.0
# Review gsKit README & LICENSE files for further details.
#
# ee/dma/src/Makefile - gsKit dma makefile.
#

DMA_CORE_OBJS = dmaKit_wait.o dmaKit_wait_fast.o dmaKit_send.o dmaKit_send_ucab.o dmaKit_send_spr.o \
	dmaKit_send_chain.o dmaKit_send_chain_ucab.o dmaKit_send_chain_spr.o dmaKit_get_spr.o

DMA_INIT_OBJS = dmaKit_init.o dmaKit_chan_init.o

DMA_SPR_OBJS = dmaKit_spr_begin.o dmaKit_spr_end.o dmaKit_spr_alloc.o

EE_OBJS = $(DMA_CORE_OBJS) $(DMA_INIT_OBJS) $(DMA_SPR_OBJS)

EE_CFLAGS += -Wno-unused

all: $(EE_OBJS)
	$(EE_AR) rcs $(LIBDMAKIT) $(EE_OBJS)

clean:
	rm -f $(EE_OBJS)

install: all

$(DMA_CORE_OBJS:%=$(EE_OBJS_DIR)%): $(EE_SRC_DIR)dmaCore.c
	$(EE_C_COMPILE) -DF_$(*:$(EE_OBJS_DIR)%=%) $< -c -o $@

$(DMA_INIT_OBJS:%=$(EE_OBJS_DIR)%): $(EE_SRC_DIR)dmaInit.c
	$(EE_C_COMPILE) -DF_$(*:$(EE_OBJS_DIR)%=%) $< -c -o $@

$(DMA_SPR_OBJS:%=$(EE_OBJS_DIR)%): $(EE_SRC_DIR)dmaSpr.c
	$(EE_C_COMPILE) -DF_$(*:$(EE_OBJS_DIR)%=%) $< -c -o $@

include ../../../Makefile.pref
include ../../../Makefile.global