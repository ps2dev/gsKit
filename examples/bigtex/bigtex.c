//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// bigtex.c - Example demonstrating gsKit texture operation.
//

#include "gsKit.h"
#include "dmaKit.h"
#include "malloc.h"

#include "gsToolkit.h"

int main(void)
{
	GSGLOBAL *gsGlobal = gsKit_init_global();
	GSTEXTURE bigtex;
	u64 Black = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x00,0x00);
	u64 TexCol = GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00);

//	gsGlobal->DoubleBuffering = GS_SETTING_OFF;
	gsGlobal->ZBuffering = GS_SETTING_OFF;

	dmaKit_init(D_CTRL_RELE_OFF,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);
	dmaKit_chan_init(DMA_CHANNEL_FROMSPR);
	dmaKit_chan_init(DMA_CHANNEL_TOSPR);

	gsKit_init_screen(gsGlobal);

        bigtex.Width = 640;
        bigtex.Height = 480;
        bigtex.PSM = GS_PSM_CT24;
        bigtex.Filter = GS_FILTER_NEAREST;

//	gsKit_texture_raw(gsGlobal, &bigtex, "host:bigtex.raw");
	gsKit_texture_bmp(gsGlobal, &bigtex, "host:bigtex.bmp");
//	gsKit_texture_jpeg(gsGlobal, &bigtex, "host:bigtex.jpg");


	float x = 0.0f;
	float x2 = gsGlobal->Width;
	int shrinkx = 1;
	int shrinkx2 = 0;

	gsKit_mode_switch(gsGlobal, GS_ONESHOT);

	while(1)
	{
		gsKit_clear(gsGlobal, Black);

		gsKit_set_clamp(gsGlobal, GS_CMODE_CLAMP);

		gsKit_prim_sprite_striped_texture(gsGlobal, &bigtex, x,  // X1
//		gsKit_prim_sprite_texture(gsGlobal, &bigtex, x,  // X1
							     0.0f,  // Y2
							     0.0f,  // U1
							     0.0f,  // V1
							     x2, // X2
							     bigtex.Height, // Y2
							     bigtex.Width, // U2
							     bigtex.Height, // V2
							     2,
							     TexCol);

		gsKit_queue_exec(gsGlobal);

		gsKit_sync_flip(gsGlobal);

		if(shrinkx)
		{
			if(x < 78.0f)
				x += 0.25f;
			else
				shrinkx = 0;
		}
		else
		{
			if(x > 0.0f)
				x -= 0.25f;
			else
				shrinkx = 1;
		}

                if(shrinkx2)
                {
                        if(x2 < gsGlobal->Width)
                                x2 += 0.25f;
                        else
                                shrinkx2 = 0;
                }
                else
                {
                        if(x2 > (gsGlobal->Width - 78.0f))
                                x2 -= 0.25f;
                        else
                                shrinkx2 = 1;
                }

	}

	return 0;
}
