//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// dmaArrays.h - Array definitions for dmaKit
//

#ifndef __DMA_ARRAYS_H
#define __DMA_ARRAYS_H

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

static u8 DMA_TAG_ENABLE[10] = { 1, 1, 0, 1, 1, 1 ,1 ,1, 1, 1 };

static char* DMA_NAME[10] = { "VIF0", "VIF1", "GIF", "fromIPU", "toIPU", \
                              "SIF0", "SIF1", "SIF2", "fromSPR", "toSPR" };

static void* DMA_SPR = (void*)SPR_START;

#endif /* __DMA_ARRAYS_H__ */
