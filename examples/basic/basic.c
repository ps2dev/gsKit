//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// basic.c - Example demonstrating basic gsKit operation.
//

#include "gsKit.h"
#include "dmaKit.h"

int main(void)
{
	dmaKit_init(D_CTRL_RELE_ON,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);

	gsKit_init(GS_NONINTERLACED, GS_MODE_DTV_480P, GS_FRAME);

	// Wait for a the channel to be clear. (dmaKit_send does this automagicly)
	dmaKit_wait(DMA_CHANNEL_GIF, 0);

	unsigned White = GS_SETREG_RGBA(0x00,0x00,0x00,0x80);

	gsKit_prim_sprite(50, 50, 300, 300, 1, White);

	while(1);

	return 0;
}
