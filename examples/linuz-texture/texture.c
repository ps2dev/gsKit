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
#include "malloc.h"

extern char testorig[];
extern unsigned int   image_clut32[];
extern unsigned char  image_pixel[];

int main(void)
{
	u64 White, Black, Red, Green, Blue, BlueTrans, RedTrans, GreenTrans, WhiteTrans;
	GSTEXTURE tex;
	GSTEXTURE tex8;
	GSGLOBAL *gsGlobal = gsKit_init_global();

	dmaKit_init(D_CTRL_RELE_ON,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);

	White = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x00,0x00);
	Black = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x00,0x00);
	Red = GS_SETREG_RGBAQ(0xFF,0x00,0x00,0x00,0x00);
	Green = GS_SETREG_RGBAQ(0x00,0xFF,0x00,0x00,0x00);
	Blue = GS_SETREG_RGBAQ(0x00,0x00,0xFF,0x00,0x00);

	BlueTrans = GS_SETREG_RGBAQ(0x00,0x00,0xFF,0x40,0x00);
	RedTrans = GS_SETREG_RGBAQ(0xFF,0x00,0x00,0x60,0x00);
	GreenTrans = GS_SETREG_RGBAQ(0x00,0xFF,0x00,0x50,0x00);
	WhiteTrans = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x50,0x00);

	gsGlobal->PSM = GS_PSM_CT32;
	gsGlobal->Test->ZTE = 0;

	gsKit_init_screen(gsGlobal);

	tex.Width = 256;
	tex.Height = 256;
	tex.PSM = GS_PSM_CT24;
	tex.Mem = testorig;
	tex.Vram = 0x2000*256;
	gsKit_texture_upload(gsGlobal, &tex);

	tex8.Width = 256;
	tex8.Height = 256;
	tex8.PSM = GS_PSM_T8;
	tex8.Mem = image_pixel;
	tex8.Vram = 0x3000*256;
	tex8.Clut = image_clut32;
	tex8.VramClut = 0x3800*256;
	gsKit_texture_upload(gsGlobal, &tex8);

	
	do
	{
		gsKit_clear(gsGlobal, White);

		gsKit_prim_sprite_texture(gsGlobal, &tex, 0, 0, 0, 0, 256, 256, 256, 256, 0, 0x80808080);
		gsKit_prim_sprite_texture(gsGlobal, &tex8, 256, 0, 0, 0, 512, 256, 256, 256, 0, 0x80808080);

		gsKit_sync_flip(gsGlobal);
	}while(1);
	
	return 0;
}

