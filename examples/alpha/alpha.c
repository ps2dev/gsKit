//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// alpha.c - Example demonstrating gsKit alpha blending operation.
//

#include <stdio.h>
#include <malloc.h>

#include <gsKit.h>
#include <dmaKit.h>
#include <gsToolkit.h>

int main(int argc, char *argv[])
{
	GSGLOBAL *gsGlobal = gsKit_init_global();
	// GS_MODE_VGA_640_60
#ifdef HAVE_LIBTIFF
	GSTEXTURE Sprite;
    u64 TexCol = GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00);
#endif
	u64 White = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x00,0x00);
    u64 Red = GS_SETREG_RGBAQ(0xFF,0x00,0x00,0x00,0x00);
    u64 Green = GS_SETREG_RGBAQ(0x00,0xFF,0x00,0x00,0x00);
	u64 Blue = GS_SETREG_RGBAQ(0x00,0x00,0xFF,0x00,0x00);
	u64 BlueTrans = GS_SETREG_RGBAQ(0x00,0x00,0xFF,0x40,0x00);

        float x = 10;
        float y = 10;
        float width = 150;
        float height = 150;

	float VHeight = gsGlobal->Height;

	gsGlobal->PSM = GS_PSM_CT24;
	gsGlobal->PSMZ = GS_PSMZ_16S;
	// gsGlobal->DoubleBuffering = GS_SETTING_OFF;
	// gsGlobal->ZBuffering = GS_SETTING_OFF;

	dmaKit_init(D_CTRL_RELE_OFF,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);

	gsGlobal->PrimAlphaEnable = GS_SETTING_ON;

	gsKit_init_screen(gsGlobal);
#ifdef HAVE_LIBTIFF
	Sprite.Delayed = 1;
	if(gsKit_texture_tiff(gsGlobal, &Sprite, "host:alpha.tiff") < 0)
	{
		printf("Loading Failed!\n");
	}
#endif
	gsKit_mode_switch(gsGlobal, GS_PERSISTENT);

	while(1)
	{
                if( y <= 10  && (x + width) < (gsGlobal->Width - 10))
                        x+=10;
                else if( (y + height)  <  (VHeight - 10) && (x + width) >= (gsGlobal->Width - 10) )
                        y+=10;
                else if( (y + height) >=  (VHeight - 10) && x > 10 )
                        x-=10;
                else if( y > 10 && x <= 10 )
                        y-=10;

		gsKit_clear(gsGlobal, White);

		gsKit_prim_quad_gouraud(gsGlobal, 250.0f, 50.0f, 250.0f, 400.0f,
						  400.0f, 50.0f, 400.0f, 400.0f,
						  1, Red, Green, Blue, White);

		gsKit_prim_sprite(gsGlobal, x, y, x + width, y + height, 2, BlueTrans);

		gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0,1,0,1,0), 0);
		gsKit_set_test(gsGlobal, GS_ATEST_OFF);
#ifdef HAVE_LIBTIFF
		gsKit_TexManager_bind(gsGlobal, &Sprite);
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

		gsKit_queue_reset(gsGlobal->Per_Queue);

		gsKit_TexManager_nextFrame(gsGlobal);
	}

	return 0;
}
