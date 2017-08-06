//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2017 - Rick "Maximus32" Gaiser <rgaiser@gmail.com>
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// coverflow.c - Example demonstrating gsKit texture manager.
//

#include <stdio.h>
#include <malloc.h>

#include <gsKit.h>
#include <dmaKit.h>
#include <gsToolkit.h>


// DVD cover size: 129x184mm
#define COVER_HEIGHT (450.0f)
#define COVER_WIDTH  (COVER_HEIGHT*129.0f/184.0f)
const float fCoverHeight   = COVER_HEIGHT;
const float fCoverWidth    = COVER_WIDTH;
const float fCoverDistance = COVER_WIDTH + 25.0f;

#define TEXTURE_COUNT 5


void drawCover(GSGLOBAL *gsGlobal, GSTEXTURE * tex, float fx, float fy)
{
	u64 TexCol1 = GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00);
	u64 TexCol2 = GS_SETREG_RGBAQ(0x80,0x80,0x80,0x20,0x00);

	gsKit_TexManager_bind(gsGlobal, tex);

	// Draw normal
	gsKit_prim_sprite_texture(gsGlobal, tex,
						fx,  // X1
						fy - fCoverHeight,  // Y2
						0.0f,  // U1
						0.0f,  // V1
						fx + fCoverWidth, // X2
						fy, // Y2
						tex->Width, // U2
						tex->Height, // V2
						2,
						TexCol1);

	// Draw upside down
	gsKit_prim_sprite_texture(gsGlobal, tex,
						fx,  // X1
						fy + fCoverHeight,  // Y2
						0.0f,  // U1
						0.0f,  // V1
						fx + fCoverWidth, // X2
						fy, // Y2
						tex->Width, // U2
						tex->Height, // V2
						2,
						TexCol2);
}

int main(int argc, char *argv[])
{
	GSGLOBAL *gsGlobal = gsKit_init_global();
	GSTEXTURE Tex[TEXTURE_COUNT];
	u64 Black = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x80,0x00);
    int iPassCount;
    float fXOff = 0.0;
    float fXOffCover;
    int iCover;
    int iCoverStart = 0;
    int i;
    char filename[80];

	gsGlobal->Mode = GS_MODE_DTV_720P;
	gsGlobal->Interlace = GS_NONINTERLACED;
	gsGlobal->Field = GS_FRAME;
	gsGlobal->Width = 1280;
	gsGlobal->Height = 720;
	iPassCount = 3;

	gsGlobal->PSM = GS_PSM_CT16S;
	gsGlobal->PSMZ = GS_PSMZ_16S;
	gsGlobal->Dithering = GS_SETTING_ON;
	gsGlobal->DoubleBuffering = GS_SETTING_ON;
	gsGlobal->ZBuffering = GS_SETTING_ON;

	dmaKit_init(D_CTRL_RELE_OFF,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);

	gsKit_hires_init_screen(gsGlobal, iPassCount);

	// Load textures
	for (i = 0; i < TEXTURE_COUNT; i++) {
		Tex[i].Delayed = 1;
		snprintf(filename, 80, "host:covers/game%d_%d_%d_08bit.bmp", i+1, 512, 512);
		gsKit_texture_bmp(gsGlobal, &Tex[i], filename);
		Tex[i].Filter = GS_FILTER_LINEAR;
	}

	// Clear static loaded textures, use texture manager instead
	gsKit_vram_clear(gsGlobal);

	gsKit_set_clamp(gsGlobal, GS_CMODE_CLAMP);

	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0, 1, 0, 1, 128), 0);
	gsGlobal->PrimAlphaEnable = GS_SETTING_ON;

	while(1)
	{
		gsKit_clear(gsGlobal, Black);

		// Draw coverflow
		iCover = iCoverStart;
		for (fXOffCover = fXOff; fXOffCover < gsGlobal->Width; fXOffCover += fCoverDistance) {
			drawCover(gsGlobal, &Tex[iCover], fXOffCover, (gsGlobal->Height/4)*3);
			iCover = iCover >= (TEXTURE_COUNT-1) ? 0 : iCover+1;
		}

		// Advance coverflow
		fXOff -= 5.0f;
		if (fXOff <= -fCoverDistance) {
			fXOff += fCoverDistance;
			iCoverStart = iCoverStart >= (TEXTURE_COUNT-1) ? 0 : iCoverStart+1;
		}

		gsKit_hires_sync(gsGlobal);
		gsKit_hires_flip(gsGlobal);
		gsKit_TexManager_nextFrame(gsGlobal);
	}

	return 0;
}
