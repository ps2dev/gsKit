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

void *dmaKit_spr_begin(void)
{
	printf("Begining Scratchpad Chain\n");
	return (void *)dmaKit_spr_alloc(100 * 16);
}

int dmaKit_spr_end(void *data, void *StorePtr)
{
	int q_size;
	printf("Ending Scratchpad Chain\n");
	q_size = (u32)data - (u32)StorePtr;
	return q_size >> 4;
}

void *dmaKit_spr_alloc(int size)
{
	void *p_spr = DMA_SPR;
	printf("Allocating Scratchpad Memory\n");
	if( DMA_SPR + size >= ( ((void*)0x70000000)+16*1024 ) )
		DMA_SPR = (void*)SPR_START;   

	p_spr = DMA_SPR;
	DMA_SPR += size;
         
	printf("Scratchpad Memory Allocated\n");
	return p_spr;
}

