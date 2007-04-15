//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// font.c - Example demonstrating basic font operation.
//

#include "gsKit.h"
#include "dmaKit.h"
#include "malloc.h"

int main(void)
{
	u64 White, Black, BlackFont, WhiteFont, RedFont, GreenFont, BlueFont, BlueTrans, RedTrans, GreenTrans, WhiteTrans;
	GSGLOBAL *gsGlobal = gsKit_init_global(GS_MODE_NTSC);

	GSFONT *gsFont = gsKit_init_font(GSKIT_FTYPE_FNT, "host:arial.fnt");

	dmaKit_init(D_CTRL_RELE_OFF,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);
	dmaKit_chan_init(DMA_CHANNEL_FROMSPR);
	dmaKit_chan_init(DMA_CHANNEL_TOSPR);
	
	Black = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x00,0x00);
	White = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x00,0x00);
	
	WhiteFont = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x80,0x00);
	BlackFont = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x80,0x00);
	RedFont = GS_SETREG_RGBAQ(0xFF,0x00,0x00,0x80,0x00);
	GreenFont = GS_SETREG_RGBAQ(0x00,0xFF,0x00,0x80,0x00);
	BlueFont = GS_SETREG_RGBAQ(0x00,0x00,0xFF,0x80,0x00);

	BlueTrans = GS_SETREG_RGBAQ(0x00,0x00,0xFF,0x40,0x00);
	RedTrans = GS_SETREG_RGBAQ(0xFF,0x00,0x00,0x60,0x00);
	GreenTrans = GS_SETREG_RGBAQ(0x00,0xFF,0x00,0x50,0x00);
	WhiteTrans = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x50,0x00);

	gsGlobal->PrimAlpha = GS_BLEND_FRONT2BACK;

	gsKit_init_screen(gsGlobal);

        gsKit_mode_switch(gsGlobal, GS_PERSISTENT);

	gsKit_font_upload(gsGlobal, gsFont);
	
	gsKit_clear(gsGlobal, Black);

	gsKit_font_print(gsGlobal, gsFont, 50, 50, 1, WhiteFont, "Hello World!");

	gsKit_font_print(gsGlobal, gsFont, 50, 80, 1, RedFont, "red");
	gsKit_font_print(gsGlobal, gsFont, 50, 110, 1, GreenFont, "green");
	gsKit_font_print(gsGlobal, gsFont, 50, 140, 1, BlueFont, "blue");

	gsKit_font_print(gsGlobal, gsFont, 100, 200, 2, WhiteFont, "Testing 1\n"\
							       "Testing 2\n"\
							       "Testing 3\n"\
							       "Testing 4\n"\
							       "Testing 5\n"\
							       "Testing 6\n"\
							       "Testing 7\n"\
							       "Testing 8\n"\
							       "Testing 9\n"\
							       "Testing 10\n");
	while(1)
	{
		gsKit_sync_flip(gsGlobal);

		gsKit_queue_exec(gsGlobal);
	}
	
	return 0;
}
