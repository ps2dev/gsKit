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

static u32 DMA_MADR[10] = { 0x10008010, 0x10009010, 0x1000A010, 0x1000B010, \
			    0x1000B410, 0x1000C010, 0x1000C410, 0x1000C810, \
			    0x1000D010, 0x1000D410 };
				
static u32 DMA_SIZE[10] = { 0x10008020, 0x10009020, 0x00000000, 0x00000000, \
			    0x00000000, 0x00000000, 0x00000000, 0x00000000, \
			    0x00000000, 0x00000000 };

static u32 DMA_TADR[10] = { 0x10008030, 0x10009030, 0x1000A030, 0x00000000, \
			    0x1000B430, 0x00000000, 0x1000C430, 0x00000000, \
			    0x00000000, 0x1000D430 };

static u32 DMA_ASR0[10] = { 0x10008040, 0x10009040, 0x1000A040, 0x00000000, \
			    0x00000000, 0x00000000, 0x00000000, 0x00000000, \
			    0x00000000, 0x00000000 };

static u32 DMA_ASR1[10] = { 0x10008050, 0x10009050, 0x1000A050, 0x00000000, \
			    0x00000000, 0x00000000, 0x00000000, 0x00000000, \
			    0x00000000, 0x00000000 };

static u32 DMA_SADR[10] = { 0x10008080, 0x10009080, 0x1000A080, 0x00000000, \
                            0x00000000, 0x00000000, 0x00000000, 0x00000000, \
			    0x1000D080, 0x1000D480 };

static u32 DMA_QWC[10] =  { 0x00000000, 0x00000000, 0x1000A020, 0x1000B020, \
			    0x1000B420, 0x1000C020, 0x1000C420, 0x1000C820, \
			    0x1000D020, 0x1000D420 };


static char* DMA_NAME[10] = { "VIF0", "VIF1", "GIF", "fromIPU", "toIPU", \
		              "SIF0", "SIF1", "SIF2", "fromSPR", "toSPR" };

#define DMA_SET_CHCR(DIR,MODE,ASP,TTE,TIE,STR,TAG)             \
	(u32)((DIR) & 0x00000001) <<  0 | (u32)((MODE) & 0x00000003) <<  2 | \
	(u32)((ASP) & 0x00000003) <<  4 | (u32)((TTE ) & 0x00000001) <<  6 | \
	(u32)((TIE) & 0x00000001) <<  7 | (u32)((STR ) & 0x00000001) <<  8 | \
	(u32)((TAG) & 0x0000FFFF) << 16

#define DMA_SET_MADR(A,B) \
	(u32)((A) & 0x7FFFFFFF) <<  0 | \
	(u32)((B) & 0x00000001) << 31

#define DMA_SET_TADR(A,B) \
	(u32)((A) & 0x7FFFFFFF) <<  0 | \
	(u32)((B) & 0x00000001) << 31

#define DMA_SET_ASR0(A,B) \
	(u32)((A) & 0x7FFFFFFF) <<  0 | \
	(u32)((B) & 0x00000001) << 31

#define DMA_SET_ASR1(A,B) \
	(u32)((A) & 0x7FFFFFFF) <<  0 | \
	(u32)((B) & 0x00000001) << 31

#define DMA_SET_SADR(A) \
	(u32)((A) & 0x00003FFF) <<  0

#define DMA_SET_SIZE(A) \
	(u32)((A) & 0x0000FFFF) <<  0

int dmaKit_wait(unsigned int channel, unsigned int timeout);

#endif /* __DMACORE_H__ */
