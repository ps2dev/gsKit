//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// dmaCore.h - Header for dmaCore.c
//

#ifndef __DMACORE_H__
#define __DMACORE_H__

#include "dmaKit.h"

static u32 DMA_CHCR[10] = { 0x10008000, 0x10009000, 0x1000A000, 0x1000B000, \
			    0x1000B400, 0x1000C000, 0x1000C400, 0x1000C800, \
			    0x1000D000, 0x1000D400 };

static char* DMA_CHAN_NAME[10] = { "VIF0", "VIF1", "GIF", "fromIPU", "toIPU", \
				   "SIF0", "SIF1", "SIF2", "fromSPR", "toSPR" };

int dmaKit_wait(unsigned int channel, unsigned int timeout);

#endif /* __DMACORE_H__ */
