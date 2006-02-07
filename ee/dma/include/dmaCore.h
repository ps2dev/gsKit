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

#define DMA_MAX_SIZE    0xFFFF

#define DMA_REFE        0x0
#define DMA_CNT         0x1
#define DMA_NEXT        0x2
#define DMA_REF         0x3
#define DMA_REFS        0x4
#define DMA_CALL        0x5
#define DMA_RET         0x6
#define DMA_END         0x7
        
#define DMA_TAG(QWC,PCE,ID,IRQ,ADDR,SPR) ( \
        ((u64)(QWC)  <<  0) | ((u64)(PCE) << 26) | \
        ((u64)(ID)   << 28) | ((u64)(IRQ) << 31) | \
        ((u64)(ADDR) << 32) | ((u64)(SPR) << 63))

#ifdef __cplusplus
extern "C" {
#endif


/// DMA Wait Routine
/// This routine checks and waits for the DMAC to become free.
/// The other routines in dmaKit use this to know when it's clear
/// to send data over the DMAC.
int dmaKit_wait(unsigned int channel, unsigned int timeout);

/// DMA Fast Wait Routine
/// This routine does the same as above, but using an accelerated
/// EE instruction method rather than register polling.
void dmaKit_wait_fast(unsigned int channel);

/// DMA Send Routine
/// Standard DMA send routine. 
void dmaKit_send(unsigned int channel, unsigned int timeout, void *data, unsigned int size);

/// DMA Scratchpad Send Routine
/// Similar to dmaKit_send, but reads from the scratchpad instead.
void dmaKit_send_spr(unsigned int channel, unsigned int timeout, void *data, unsigned int size);

/// DMA Chain Send Routine
/// Sends data over the DMAC using a DMA Chain.
void dmaKit_send_chain(unsigned int channel, unsigned int timeout, void *data, unsigned int size);

/// DMA Chain Scratchpad Send 
/// Sends data over the DMAC using a DMA Chain via the scratchpad.
void dmaKit_send_chain_spr(unsigned int channel, unsigned int timeout, void *data);

/// DMA Get from Scratchpad Routine
/// Transfers data from an external source to the EE.
/// (ex: Scratchpad -> EE Main Memory)
void dmaKit_get_spr(unsigned int channel, void *data, void *dest, unsigned int size);

#ifdef __cplusplus
}
#endif

#endif /* __DMACORE_H__ */
