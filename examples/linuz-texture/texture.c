//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// texture.c - Example demonstrating CLUT texture operation.
//

#include "gsKit.h"
#include "dmaKit.h"
#include "malloc.h"

extern char testorig[];
extern unsigned int   image_clut32[];
extern unsigned char  image_pixel[];

int main(void)
{
	u64 White, Black;
	GSTEXTURE tex;
	GSTEXTURE tex8;
	GSGLOBAL *gsGlobal = gsKit_init_global(GS_MODE_NTSC);

	dmaKit_init(D_CTRL_RELE_ON,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);

	White = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x00,0x00);
	Black = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x00,0x00);

	gsGlobal->PSM = GS_PSM_CT24;

	gsGlobal->ZBuffering = GS_SETTING_OFF;

	gsKit_init_screen(gsGlobal);

	tex.Width = 256;
	tex.Height = 256;
	tex.PSM = GS_PSM_CT24;
	tex.Mem = testorig;
	tex.Vram = gsKit_vram_alloc(gsGlobal, gsKit_texture_size(tex.Width, tex.Height, tex.PSM), GSKIT_ALLOC_USERBUFFER);
	tex.Filter = GS_FILTER_LINEAR;
	gsKit_texture_upload(gsGlobal, &tex);

	tex8.Width = 256;
	tex8.Height = 256;
	tex8.PSM = GS_PSM_T8;
	tex8.Mem = image_pixel;
	tex8.Vram = gsKit_vram_alloc(gsGlobal, gsKit_texture_size(tex8.Width, tex8.Height, tex8.PSM), GSKIT_ALLOC_USERBUFFER);
	tex8.Clut = image_clut32;
	tex8.ClutPSM = GS_PSM_CT32;
	tex8.VramClut = gsKit_vram_alloc(gsGlobal, gsKit_texture_size(16, 16, GS_PSM_CT32), GSKIT_ALLOC_USERBUFFER);
	tex8.Filter = GS_FILTER_LINEAR;
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

