//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// dmaInit.c - DMA initilization routines.
//

#include "dmaKit.h"
#include "kernel.h"

int dmaKit_init(unsigned int RELE, unsigned int MFD, unsigned int STS,
		unsigned int STD, unsigned int RCYC)
{
	printf("Initializing the DMAC: RELE=%i MFD=%i STS=%i STD=%i RCYC=%i\n",
		RELE, MFD, STS, STD, RCYC);
	
	*DMA_REG_CTRL = 0x00000000;
	*DMA_REG_STAT = 0x00000000;
	*DMA_REG_PCR  = 0x00000000;
	*DMA_REG_SQWC = 0x00000000;
	*DMA_REG_RBSR = 0x00000000;
	*DMA_REG_RBOR = 0x00000000;

	*DMA_REG_CTRL = DMA_SET_CTRL(1, RELE, MFD, STS, STD, RCYC);

	printf("DMAC Initialized.\n");	

	return 0;
}

int dmaKit_chan_init(unsigned int channel)
{
	if(channel >= 0 && channel <= 9)
	{
		printf("Initilizating DMA Channel %i - %s\n",channel, DMA_NAME[channel]);
	
		*(volatile u32 *)DMA_CHCR[channel] = 0x00000000;
		*(volatile u32 *)DMA_MADR[channel] = 0x00000000;
		if(DMA_SIZE[channel] > 0)
			*(volatile u32 *)DMA_SIZE[channel] = 0x00000000;
		if(DMA_TADR[channel] > 0)
			*(volatile u32 *)DMA_TADR[channel] = 0x00000000;
		if(DMA_ASR0[channel] > 0)
			*(volatile u32 *)DMA_ASR0[channel] = 0x00000000;
		if(DMA_ASR1[channel] > 0)
			*(volatile u32 *)DMA_ASR1[channel] = 0x00000000;
		if(DMA_SADR[channel] > 0)
			*(volatile u32 *)DMA_SADR[channel] = 0x00000000;
		if(DMA_QWC[channel] > 0)
			*(volatile u32 *)DMA_QWC[channel]  = 0x00000000; 
	}
	else
	{
		printf("Invalid DMA Channel Specified: %i\n",channel);
		printf("Failed to Initialize DMA Channel.\n");
		return -1;
	}

	printf("DMA Channel Initialized.\n");	

	return 0;
}
