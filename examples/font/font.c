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

#include "stdio.h"
#include "gsKit.h"
#include "dmaKit.h"
#include "malloc.h"

#include "arial.c"


int main(void)
{
	u64 White, Black, Red, Green, Blue, BlueTrans, RedTrans, GreenTrans, WhiteTrans, Texture;
	GSGLOBAL *gsGlobal = gsKit_init_global(GS_MODE_NTSC);

	GSFONT *gsFont;// = gsKit_init_font(GSKIT_FTYPE_BMP_DAT, "NULL");

	dmaKit_init(D_CTRL_RELE_ON,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);

	Texture = GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00);

	White = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x00,0x00);
	Black = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x00,0x00);
	Red = GS_SETREG_RGBAQ(0xFF,0x00,0x00,0x00,0x00);
	Green = GS_SETREG_RGBAQ(0x00,0xFF,0x00,0x00,0x00);
	Blue = GS_SETREG_RGBAQ(0x00,0x00,0xFF,0x00,0x00);

	BlueTrans = GS_SETREG_RGBAQ(0x00,0x00,0xFF,0x40,0x00);
	RedTrans = GS_SETREG_RGBAQ(0xFF,0x00,0x00,0x60,0x00);
	GreenTrans = GS_SETREG_RGBAQ(0x00,0xFF,0x00,0x50,0x00);
	WhiteTrans = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x50,0x00);

	gsKit_init_screen(gsGlobal);
	gsKit_clear(gsGlobal, Black);

/*	gsFont = gsKit_init_font(GSKIT_FTYPE_FNT, "host0:arial.fnt");
	if (gsFont == NULL) {
		gsFont = gsKit_init_font(GSKIT_FTYPE_FNT, "cdrom0:\\ARIAL.FNT;1");
		if (gsFont == NULL) {
			printf("Error loading arial.fnt\n");
			return 1;
		}
	}*/
	gsFont = gsKit_init_font_raw(GSKIT_FTYPE_FNT, arial_fnt, size_arial_fnt);
//	gsFont->Path_BMP = "host:fonts/tahoma.bmp";
//	gsFont->Path_DAT = "host:fonts/tahoma.dat";

	gsKit_font_upload(gsGlobal, gsFont);

	while(1){
		gsKit_clear(gsGlobal, Black);

		gsKit_font_print(gsGlobal, gsFont, 50, 50, 1, White, "Hello World!");

		gsKit_font_print(gsGlobal, gsFont, 50, 80, 1, Red, "red");
		gsKit_font_print(gsGlobal, gsFont, 50, 110, 1, Green, "green");
		gsKit_font_print(gsGlobal, gsFont, 50, 140, 1, Blue, "blue");

		gsKit_font_print(gsGlobal, gsFont, 100, 200, 1, White, "Testing 1\n"\
								       "Testing 2\n"\
								       "Testing 3\n"\
								       "Testing 4\n"\
								       "Testing 5\n"\
								       "Testing 6\n"\
								       "Testing 7\n"\
								       "Testing 8\n"\
								       "Testing 9\n"\
								       "Testing 10\n");

		gsKit_sync_flip(gsGlobal);
	}
	
	return 0;
}
