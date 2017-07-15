//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// pixelperfect.c - Example demonstrating gsKit texture operation.
//                  Aligning the texels exactly to pixels
//

#include <stdio.h>
#include <malloc.h>

#include <gsKit.h>
#include <dmaKit.h>
#include <gsToolkit.h>


int main(int argc, char *argv[])
{
	GSGLOBAL *gsGlobal = gsKit_init_global();
	GSFONTM *gsFontM = gsKit_init_fontm();
	GSTEXTURE tx128, tx132;
	char tempstr[256];
	int i=0;
	int toggle=0;
	float fOffsetX = 0.0f;
	float fOffsetY = 0.0f;
	float fOffsetU = 0.0f;
	float fOffsetV = 0.0f;

	u64 Black = GS_SETREG_RGBA(0x00,0x00,0x00,0x80);
	u64 White = GS_SETREG_RGBA(0xFF,0xFF,0xFF,0x80);
	u64 TexCol = GS_SETREG_RGBA(0x80,0x80,0x80,0x80);

	gsGlobal->Mode = GS_MODE_DTV_480P;
	gsGlobal->Interlace = GS_NONINTERLACED;
	gsGlobal->Field = GS_FRAME;
	gsGlobal->Width = 704;
	gsGlobal->Height = 480;

	gsGlobal->PSM = GS_PSM_CT24;
	gsGlobal->PSMZ = GS_PSMZ_16S;
	gsGlobal->DoubleBuffering = GS_SETTING_ON;
	gsGlobal->ZBuffering = GS_SETTING_OFF;

	dmaKit_init(D_CTRL_RELE_OFF,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);

	gsKit_init_screen(gsGlobal);
	gsKit_fontm_upload(gsGlobal, gsFontM);

	// Load textures
	tx128.Delayed = 0;
	tx132.Delayed = 0;
	gsKit_texture_png(gsGlobal, &tx128, "host:128x128.png");
	gsKit_texture_png(gsGlobal, &tx132, "host:132x132.png");

	gsKit_set_clamp(gsGlobal, GS_CMODE_CLAMP);
	gsKit_set_test(gsGlobal, GS_ZTEST_OFF);
	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0, 1, 0, 1, 0), 0);
	gsGlobal->PrimAlphaEnable = GS_SETTING_ON;

	while(1) {
		gsKit_clear(gsGlobal, White);

		i++;
		if (i >= 60) {
			i=0;
			toggle = toggle ? 0 : 1;
		}

		if (toggle) {
			// Default texture mapping
			fOffsetX = 0.0f;
			fOffsetY = 0.0f;
			fOffsetU = 0.0f;
			fOffsetV = 0.0f;
		}
		else {
			// Pixel perfect texture mapping:
			// - Move to the upper-left corner of the pixel
			fOffsetX = -0.5f;
			fOffsetY = -0.5f;
			fOffsetU = 0.0f;
			fOffsetV = 0.0f;
		}

		gsKit_fontm_print_scaled(gsGlobal, gsFontM, 20,  20, 2, 0.5f, Black, "GS_FILTER_NEAREST");
		gsKit_fontm_print_scaled(gsGlobal, gsFontM, 20, 190, 2, 0.5f, Black, "GS_FILTER_LINEAR");

		tx128.Filter = GS_FILTER_NEAREST;
		gsKit_prim_sprite_texture(gsGlobal, &tx128,
								fOffsetX+ 20.0f, // X1
								fOffsetY+ 40.0f, // Y2
								fOffsetU+  0.0f, // U1
								fOffsetV+  0.0f, // V1
								fOffsetX+tx128.Width  + 20.0f, // X2
								fOffsetY+tx128.Height + 40.0f, // Y2
								fOffsetU+tx128.Width,  // U2
								fOffsetV+tx128.Height, // V2
								2,
								TexCol);

		tx132.Filter = GS_FILTER_NEAREST;
		gsKit_prim_sprite_texture(gsGlobal, &tx132,
								fOffsetX+170.0f, // X1
								fOffsetY+ 40.0f, // Y2
								fOffsetU+  2.0f, // U1
								fOffsetV+  2.0f, // V1
								fOffsetX+tx132.Width  - 4.0f + 170.0f, // X2
								fOffsetY+tx132.Height - 4.0f +  40.0f, // Y2
								fOffsetU+tx132.Width  - 2.0f, // U2
								fOffsetV+tx132.Height - 2.0f, // V2
								2,
								TexCol);

		tx128.Filter = GS_FILTER_LINEAR;
		gsKit_prim_sprite_texture(gsGlobal, &tx128,
								fOffsetX+ 20.0f, // X1
								fOffsetY+210.0f, // Y2
								fOffsetU+  0.0f, // U1
								fOffsetV+  0.0f, // V1
								fOffsetX+tx128.Width  +  20.0f, // X2
								fOffsetY+tx128.Height + 210.0f, // Y2
								fOffsetU+tx128.Width,  // U2
								fOffsetV+tx128.Height, // V2
								2,
								TexCol);

		tx132.Filter = GS_FILTER_LINEAR;
		gsKit_prim_sprite_texture(gsGlobal, &tx132,
								fOffsetX+170.0f, // X1
								fOffsetY+210.0f, // Y2
								fOffsetU+  2.0f, // U1
								fOffsetV+  2.0f, // V1
								fOffsetX+tx132.Width  - 4.0f + 170.0f, // X2
								fOffsetY+tx132.Height - 4.0f + 210.0f, // Y2
								fOffsetU+tx132.Width  - 2.0f, // U2
								fOffsetV+tx132.Height - 2.0f, // V2
								2,
								TexCol);

		// Magnified to 350x350 pixels
		tx132.Filter = GS_FILTER_LINEAR;
		gsKit_prim_sprite_texture(gsGlobal, &tx132,
								fOffsetX+320.0f, // X1
								fOffsetY+ 40.0f, // Y2
								fOffsetU+  2.0f, // U1
								fOffsetV+  2.0f, // V1
								fOffsetX+350.0f + 320.0f, // X2
								fOffsetY+350.0f +  40.0f, // Y2
								fOffsetU+tx132.Width  - 2.0f, // U2
								fOffsetV+tx132.Height - 2.0f, // V2
								2,
								TexCol);

		sprintf(tempstr, "fOffsetXY = %.3f %.3f", fOffsetX, fOffsetY);
		gsKit_fontm_print_scaled(gsGlobal, gsFontM, 20, 400, 2, 0.5f, Black, tempstr);
		sprintf(tempstr, "fOffsetUV = %.3f %.3f", fOffsetU, fOffsetV);
		gsKit_fontm_print_scaled(gsGlobal, gsFontM, 20, 430, 2, 0.5f, Black, tempstr);

		gsKit_queue_exec(gsGlobal);
		gsKit_sync_flip(gsGlobal);
	}

	return 0;
}
