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

	if(channel == DMA_CHANNEL_GIF)
	{
		*GIF_CHCR = 0x00000000;
		*GIF_MADR = 0x00000000;
		*GIF_SIZE = 0x00000000;
		*GIF_TADR = 0x00000000;
		*GIF_ASR0 = 0x00000000;
		*GIF_ASR1 = 0x00000000;
		*GIF_SADR = 0x00000000;
	}
	else if(channel == DMA_CHANNEL_VIF0)
	{
		*VIF0_CHCR = 0x00000000;
		*VIF0_MADR = 0x00000000;
		*VIF0_SIZE = 0x00000000;
		*VIF0_TADR = 0x00000000;
		*VIF0_ASR0 = 0x00000000;
		*VIF0_ASR1 = 0x00000000;
		*VIF0_SADR = 0x00000000;
	}	
	else if(channel == DMA_CHANNEL_VIF1)
	{
		*VIF1_CHCR = 0x00000000;
		*VIF1_MADR = 0x00000000;
		*VIF1_SIZE = 0x00000000;
		*VIF1_TADR = 0x00000000;
		*VIF1_ASR0 = 0x00000000;
		*VIF1_ASR0 = 0x00000000;
		*VIF1_SADR = 0x00000000;
	}
	else if(channel == DMA_CHANNEL_FROMIPU)
	{
		*FROMIPU_CHCR = 0x00000000;
		*FROMIPU_MADR = 0x00000000;
		*FROMIPU_QWC  = 0x00000000;
	}
	else if(channel == DMA_CHANNEL_TOIPU)
	{
		*TOIPU_CHCR = 0x00000000;
		*TOIPU_MADR = 0x00000000;
		*TOIPU_QWC  = 0x00000000;
		*TOIPU_TADR = 0x00000000;
	}
	else if(channel == DMA_CHANNEL_SIF0)
	{
		*SIF0_CHCR = 0x00000000;
		*SIF0_MADR = 0x00000000;
		*SIF0_QWC = 0x00000000;
	}
	else if(channel == DMA_CHANNEL_SIF1)
	{
		*SIF1_CHCR = 0x00000000;
		*SIF1_MADR = 0x00000000;
		*SIF1_QWC  = 0x00000000;
		*SIF1_TADR = 0x00000000;
	}
	else if(channel == DMA_CHANNEL_SIF2)
	{
		*SIF2_CHCR = 0x00000000;
		*SIF2_MADR = 0x00000000;
		*SIF2_QWC = 0x00000000;
	}
	else if(channel == DMA_CHANNEL_FROMSPR)
	{
		*FROMSPR_CHCR = 0x00000000;
		*FROMSPR_MADR = 0x00000000;
		*FROMSPR_QWC = 0x00000000;
		*FROMSPR_SADR = 0x00000000;
	}
	else if(channel == DMA_CHANNEL_TOSPR)
	{
		*TOSPR_CHCR = 0x00000000;
		*TOSPR_MADR = 0x00000000;
		*TOSPR_QWC = 0x00000000;
		*TOSPR_TADR = 0x00000000;
		*TOSPR_SADR = 0x00000000;
	}

	return 0;
}
