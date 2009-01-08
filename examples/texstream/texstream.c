//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// texstream.c - Example demonstrating gsKit texture streaming operation.
//

#include "gsKit.h"
#include "dmaKit.h"
#include "malloc.h"

int main(void)
{
	GSGLOBAL *gsGlobal = gsKit_init_global();
//GS_MODE_VGA_640_60
	GSTEXTURE Sprite;
	u64 White = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x00,0x00);
#ifdef HAVE_LIBPNG
	u64 TexCol = GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00);
#endif
	gsGlobal->PSM = GS_PSM_CT24;
	gsGlobal->PSMZ = GS_PSMZ_16S;
//	gsGlobal->DoubleBuffering = GS_SETTING_OFF;
//	gsGlobal->ZBuffering = GS_SETTING_OFF;

	dmaKit_init(D_CTRL_RELE_OFF,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);

	gsGlobal->PrimAlphaEnable = GS_SETTING_ON;

	gsKit_init_screen(gsGlobal);

	Sprite.Delayed = GS_SETTING_ON;

#ifdef HAVE_LIBPNG
	if(gsKit_texture_png(gsGlobal, &Sprite, "host:texstream.png") < 0)
	{
		printf("Loading Failed!\n");
	}
#endif

	Sprite.Vram = gsKit_vram_alloc(gsGlobal, gsKit_texture_size(Sprite.Width, Sprite.Height, Sprite.PSM), GSKIT_ALLOC_USERBUFFER);

	gsKit_mode_switch(gsGlobal, GS_ONESHOT);

	while(1)
	{
		gsKit_clear(gsGlobal, White);

		gsKit_texture_send_inline(gsGlobal, Sprite.Mem, Sprite.Width, Sprite.Height, Sprite.Vram, Sprite.PSM, Sprite.TBW, GS_CLUT_NONE);

		gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0,1,0,1,0), 0);
		gsKit_set_test(gsGlobal, GS_ATEST_OFF);
#ifdef HAVE_LIBPNG
		gsKit_prim_sprite_texture(gsGlobal, &Sprite,	310.0f,  // X1
								50.0f,  // Y2
								0.0f,  // U1
								0.0f,  // V1
								Sprite.Width + 310.0f, // X2
								Sprite.Height +  50.0f, // Y2
								Sprite.Width, // U2
								Sprite.Height, // V2
								3,
								TexCol);
#endif
		gsKit_set_test(gsGlobal, GS_ATEST_ON);
		gsKit_set_primalpha(gsGlobal, GS_BLEND_BACK2FRONT, 0);

		gsKit_sync_flip(gsGlobal);

		gsKit_queue_exec(gsGlobal);
	}

	return 0;
}
