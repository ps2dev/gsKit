//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsCore.c - C/Assembler implimentation of all GS instructions.
//

#include "dmaKit.h"
#include "kernel.h"

int dmaKit_wait(unsigned int channel, unsigned int timeout)
{
	printf("Waiting for DMA Channel %i - %s\n",channel, DMA_CHAN_NAME[channel]);
	while((*(volatile u32 *)DMA_CHCR[channel]) & 0x00000100)
	{
		if ( timeout != 0 )
		{
			if ( timeout == 1 )
			{
				printf("Timed out waiting for DMA Channel %i - %s to be clear.\n",channel, DMA_CHAN_NAME[channel]);
				return -1;
			}
			timeout--;
		}
	}
	printf("DMA Channel %i - %s is now clear.\n",channel, DMA_CHAN_NAME[channel]);

	return 0;
}


