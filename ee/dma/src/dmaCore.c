//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// dmaCore.c - Core DMA Routines.
// Parts taken from emoon's BreakPoint Demo Library
//

#include "dmaKit.h"
#include "kernel.h"

int dmaKit_wait(unsigned int channel, unsigned int timeout)
{
	#ifdef GSKIT_DEBUG
	printf("Waiting for DMA Channel %i - %s\n",channel, DMA_NAME[channel]);
	#endif
	while((*(volatile u32 *)DMA_CHCR[channel]) & 0x00000100)
	{
		if ( timeout != 0 )
		{
			if ( timeout == 1 )
			{
				printf("Timed out waiting for DMA Channel %i - %s to be clear.\n",channel, DMA_NAME[channel]);
				return -1;
			}
			timeout--;
		}
	}
	#ifdef GSKIT_DEBUG
	printf("DMA Channel %i - %s is now clear.\n",channel, DMA_NAME[channel]);
	#endif
	
	return 0;
}

int dmaKit_send(unsigned int channel, unsigned int timeout, void *data, unsigned int size)
{
	#ifdef GSKIT_DEBUG
	printf("Sending to DMA Channel %i - %s\n",channel, DMA_NAME[channel]);
	#endif
	if(dmaKit_wait(channel, timeout) < 0 )
	{
		printf("Timed Out. Aborting Send.\n");
		return -1;
	}
	SyncDCache(data, data+size*16);

        if(DMA_QWC[channel] != 0)
		*(volatile u32 *)DMA_QWC[channel] = size;
	*(volatile u32 *)DMA_MADR[channel] = (u32)data;
	*(volatile u32 *)DMA_CHCR[channel] = DMA_SET_CHCR(1,	// Direction
							  0,	// ChainMode
							  0,	// Address Stack Pointer
							  DMA_TAG_ENABLE[channel], // Transfer DMA Tag
							  0,	// No Interrupts   
							  1,	// Start DMA
							  0 );	// Priority Control Enable??

	#ifdef GSKIT_DEBUG
	printf("Sent to DMA Channel\n");
	#endif

	return 0;
}

int dmaKit_send_spr(unsigned int channel, unsigned int timeout, void *data, unsigned int size)
{
	#ifdef GSKIT_DEBUG
	printf("Sending to DMA Channel w/ SPR %i - %s\n",channel, DMA_NAME[channel]);
	#endif
	if(dmaKit_wait(channel, timeout) < 0 )
	{
		printf("Timed Out. Aborting Send.\n");
		return -1;
	}
	SyncDCache(data, data+size*16);
	if(DMA_QWC[channel] != 0)
		*(volatile u32 *)DMA_QWC[channel] = size;
	*(volatile u32 *)DMA_MADR[channel] = (u32)data | 0x80000000;
	*(volatile u32 *)DMA_CHCR[channel] = DMA_SET_CHCR(1,    // Direction
							  0,    // ChainMode
							  0,    // Address Stack Pointer
							  DMA_TAG_ENABLE[channel], // Transfer DMA Tag
							  0,    // No Interrupts
							  1,    // Start DMA
							  0 );  // Priority Control Enable??
	#ifdef GSKIT_DEBUG
	printf("Sent to DMA Channel w/ SPR\n");
	#endif

	return 0;
}

int dmaKit_send_chain(unsigned int channel, unsigned int timeout, void *data,
		      unsigned int size)
{
	#ifdef GSKIT_DEBUG
	printf("Sending to DMA Channel in Chain Mode %i - %s\n",channel, DMA_NAME[channel]);
	#endif
        if(dmaKit_wait(channel, timeout) < 0 )
        {
                printf("Timed Out. Aborting Send.\n");
                return -1;
        }
        SyncDCache(data, data+size*16);

	if(DMA_QWC[channel] != 0)
        	*(volatile u32 *)DMA_QWC[channel] = 0;
	*(volatile u32 *)DMA_TADR[channel] = (u32)data;
        *(volatile u32 *)DMA_CHCR[channel] = DMA_SET_CHCR(1,	// Direction
							  1,	// ChainMode
							  0,	// Address Stack Pointer
							  DMA_TAG_ENABLE[channel], // Transfer DMA Tag
							  0,	// No Interrupts
							  1,	// Start DMA
							  0 );	// Priority Control Enable??

	#ifdef GSKIT_DEBUG
	printf("Sent to DMA Channel in Chain Mode\n");
	#endif
        return 0;
}

int dmaKit_send_chain_spr(unsigned int channel, unsigned int timeout, void *data,
                          unsigned int size)
{
	#ifdef GSKIT_DEBUG
	printf("Sending to DMA Channel in Chain Mode w/Scratchpad %i - %s\n",channel, DMA_NAME[channel]);
	#endif
        if(dmaKit_wait(channel, timeout) < 0 )
        {
                printf("Timed Out. Aborting Send.\n");
                return -1;                                
        }
        //SyncDCache(data, data+size*16);

	if(DMA_QWC[channel] != 0)
	        *(volatile u32 *)DMA_QWC[channel] = 0;
	if(DMA_TADR[channel] != 0)
		*(volatile u32 *)DMA_TADR[channel] = (u32)data | 0x80000000;
	*(volatile u32 *)DMA_CHCR[channel] = DMA_SET_CHCR(1, 	// Direction
							  1, 	// ChainMode
							  0, 	// Address Stack Pointer
							  DMA_TAG_ENABLE[channel], // Transfer DMA Tag
							  0,	// No Interrupts
							  1, 	// Start DMA
							  0 );	// Priority Control Enable??

	#ifdef GSKIT_DEBUG
	printf("Sent to DMA Channel in Chain Mode w/Scratchpad\n");
	#endif
	return 0;
}

