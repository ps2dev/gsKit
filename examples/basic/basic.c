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

	gsKit_init(GS_NONINTERLACED, GS_MODE_NTSC, GS_FRAME);

	// Wait for a the channel to be clear. (dmaKit_send does this automagicly)
	dmaKit_wait(DMA_CHANNEL_GIF, 0);

	unsigned White = GS_SETREG_RGBA(0xFF,0xFF,0xFF,0x00);
	unsigned Red = GS_SETREG_RGBA(0xFF,0x00,0x00,0x00);
	unsigned Green = GS_SETREG_RGBA(0x00,0xFF,0x00,0x00);
	unsigned Blue = GS_SETREG_RGBA(0x00,0x00,0xFF,0x00);

	GSGLOBAL gsGlobal;

	gsGlobal.Width = 640;
	gsGlobal.Height = 448;
	gsGlobal.Aspect = GS_ASPECT_4_3;
	gsGlobal.OffsetX = 2048;
	gsGlobal.OffsetY = 2048;
	gsGlobal.PSM = 0;
	gsGlobal.ActiveBuffer = 1;
	gsGlobal.PrimAlphaEnable = 0;
	gsGlobal.BGColor.Red = 0x00;
	gsGlobal.BGColor.Green = 0x00;
	gsGlobal.BGColor.Blue = 0x00;

	gsGlobal = gsKit_init_screen(gsGlobal);

	gsKit_clear(gsGlobal, White);

	gsGlobal = gsKit_sync_flip(gsGlobal);
	
	gsKit_prim_sprite(gsGlobal, 100, 100, 200, 200, 1, Blue);
	gsKit_prim_sprite_ztest(gsGlobal, 200, 200, 400, 400, 1, Green);

	gsGlobal = gsKit_sync_flip(gsGlobal);

	while(1);

	return 0;
}
