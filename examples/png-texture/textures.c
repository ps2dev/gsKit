//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// textures.c - Example demonstrating gsKit texture operation.
//

#include <stdio.h>

#include "gsKit.h"
#include "dmaKit.h"
#include "malloc.h"

#include "gsToolkit.h"

int main(void)
{
	GSGLOBAL *gsGlobal = gsKit_init_global();

	GSTEXTURE Tex1;
	int x = 0, y = 0;

#ifdef HAVE_LIBPNG
	u64 TexCol = GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00);
#endif
	u64 White = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x00,0x00);
	u64 BlueTrans = GS_SETREG_RGBAQ(0x00,0x00,0xFF,0x40,0x00);
	u64 Green = GS_SETREG_RGBAQ(0x00,0xFF,0x00,0x00,0x00);

	gsGlobal->PSM = GS_PSM_CT32;
	gsGlobal->PSMZ = GS_PSMZ_16S;
	gsGlobal->ZBuffering = GS_SETTING_ON;

	dmaKit_init(D_CTRL_RELE_OFF,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);
	dmaKit_chan_init(DMA_CHANNEL_FROMSPR);
	dmaKit_chan_init(DMA_CHANNEL_TOSPR);
#ifdef HAVE_LIBPNG
	printf("alpha\n");
	gsGlobal->PrimAlphaEnable = GS_SETTING_ON;
#endif
	gsKit_init_screen(gsGlobal);

	float VHeight = gsGlobal->Height;

	gsKit_clear(gsGlobal, White);
#ifdef HAVE_LIBPNG
	gsKit_texture_png(gsGlobal, &Tex1, "host:test.png");
	printf("Texture 1 Height: %i\n",Tex1.Height);
	printf("Texture 1 Width: %i\n",Tex1.Width);

	printf("Texure 1 VRAM Range = 0x%X - 0x%X\n",Tex1.Vram, Tex1.Vram +gsKit_texture_size(Tex1.Width, Tex1.Height, Tex1.PSM) - 1);
#endif
	gsKit_mode_switch(gsGlobal, GS_PERSISTENT);

	while(1)
	{

        	if( y <= 10  && (x + Tex1.Width) < (gsGlobal->Width - 10))
			x+=10;
	        else if( (y + Tex1.Height)  <  (VHeight - 10) && (x + Tex1.Width) >= (gsGlobal->Width - 10) )
	                y+=10;
	        else if( (y + Tex1.Height) >=  (VHeight - 10) && x > 10 )
	                x-=10;
	        else if( y > 10 && x <= 10 )
	                y-=10;

		gsKit_clear(gsGlobal, Green);

		gsKit_prim_sprite(gsGlobal, x, y, x + Tex1.Width, y + Tex1.Height, 1, BlueTrans);
#ifdef HAVE_LIBPNG
		gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0,1,0,1,0), 0);
		gsKit_set_test(gsGlobal, GS_ATEST_OFF);

		gsKit_prim_sprite_texture(gsGlobal, &Tex1,	0.0f,  // X1
							0.0f,  // Y2
							0.0f,  // U1
							0.0f,  // V1
							Tex1.Width, // X2
							Tex1.Height, // Y2
							Tex1.Width, // U2
							Tex1.Height, // V2
							2,
							TexCol);


                gsKit_set_test(gsGlobal, GS_ATEST_ON);
                gsKit_set_primalpha(gsGlobal, GS_BLEND_BACK2FRONT, 0);
#endif
		gsKit_sync_flip(gsGlobal);
		gsKit_queue_exec(gsGlobal);
#ifdef HAVE_LIBPNG
		gsKit_queue_reset(gsGlobal->Per_Queue);
#endif
	}

	return 0;
}
