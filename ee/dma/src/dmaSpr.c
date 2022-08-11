//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// dmaSpr.c - Interface to scratchpad.
// Parts taken from emoon's BreakPoint Demo Library
//

#include "dmaKit.h"
#include "kernel.h"
#include "dmaArrays.h"
#include <stdio.h>

#if F_dmaKit_spr_begin
void *dmaKit_spr_begin(void)
{
	#ifdef GSKIT_DEBUG
	printf("Begining Scratchpad Chain\n");
	#endif
	return (void *)dmaKit_spr_alloc(100 * 16);
}
#endif

#if F_dmaKit_spr_end
int dmaKit_spr_end(void *data, void *StorePtr)
{
	int q_size;
	#ifdef GSKIT_DEBUG
	printf("Ending Scratchpad Chain\n");
	#endif
	q_size = (u32)data - (u32)StorePtr;
	return q_size >> 4;
}
#endif

#if F_dmaKit_spr_alloc
void *dmaKit_spr_alloc(int size)
{
	void *p_spr = DMA_SPR;
	#ifdef GSKIT_DEBUG
	printf("Allocating Scratchpad Memory\n");
	#endif
	if( (u32)DMA_SPR + size >= 0x70004000)
		DMA_SPR = (void*)SPR_START;

	p_spr = DMA_SPR;
	DMA_SPR += size;
      
	#ifdef GSKIT_DEBUG
	printf("Scratchpad Memory Allocated\n");
	#endif
	return p_spr;
}
#endif

