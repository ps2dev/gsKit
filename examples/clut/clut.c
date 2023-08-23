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
#include <malloc.h>

#include <gsKit.h>
#include <dmaKit.h>
#include <gsToolkit.h>
#include <gsInline.h>
#include <gsTexture.h>

#define CLUT_SIZE 256
#define TEXTURE_WIDTH 320
#define TEXTURE_HEIGHT 240

int main(int argc, char *argv[])
{
	GSGLOBAL *gsGlobal;
	GSTEXTURE tex;
	uint32_t i, j, offset;
	uint32_t totalVertices = 2;
	uint64_t White = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x00,0x00);
	gs_rgbaq color = color_to_RGBAQ(0x80, 0x80, 0x80, 0x80, 0);

	gsGlobal = gsKit_init_global();

	gsGlobal->PSM = GS_PSM_CT24;
	gsGlobal->PSMZ = GS_PSMZ_16S;

	dmaKit_init(D_CTRL_RELE_OFF,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);

	gsKit_init_screen(gsGlobal);
	gsKit_TexManager_init(gsGlobal);
	gsKit_mode_switch(gsGlobal, GS_ONESHOT);

	tex.Width = TEXTURE_WIDTH;
	tex.Height = TEXTURE_HEIGHT;
	tex.PSM = GS_PSM_T8;
	tex.ClutPSM = GS_PSM_CT32;
	tex.Clut = memalign(128, gsKit_texture_size_ee(CLUT_SIZE, 1, tex.ClutPSM));
	tex.Mem = memalign(128, gsKit_texture_size_ee(tex.Width, tex.Height, tex.PSM));

	printf("Tex VRAM Range = 0x%X - 0x%X\n",tex.Vram, tex.Vram +gsKit_texture_size(tex.Width, tex.Height, tex.PSM) - 1);

	gsKit_set_clamp(gsGlobal, GS_CMODE_CLAMP);

	// initialize texture
	uint8_t *tex256 = (uint8_t*)tex.Mem;
	for (j = 0; j < tex.Height; ++j) {
		for (i = 0; i < tex.Width; ++i) {
			tex256[i + j * tex.Width] = j ^ i;
		}
	}
	
	GSPRIMUVPOINT *verts = (GSPRIMUVPOINT*)malloc(sizeof(GSPRIMUVPOINT) * totalVertices);
	verts[0].xyz2 = vertex_to_XYZ2(gsGlobal, 0, 0, 0);
	verts[0].rgbaq = color;
	verts[0].uv = vertex_to_UV(&tex, 0, 0);

	verts[1].xyz2 = vertex_to_XYZ2(gsGlobal, gsGlobal->Width, gsGlobal->Height, 0);
	verts[1].rgbaq = color;
	verts[1].uv = vertex_to_UV(&tex, tex.Width, tex.Height);

	offset = 0;

	while(1) {
		unsigned int* clut = tex.Clut;
		for (i = 0; i < CLUT_SIZE; ++i) {
			unsigned int j = (i + offset)&0xff;
			*(clut++) = (j << 24)|(j << 16)|(j << 8)|(j);
		}

		gsKit_clear(gsGlobal, White);
		gsKit_TexManager_invalidate(gsGlobal, &tex);
		gsKit_TexManager_bind(gsGlobal, &tex);
		gskit_prim_list_sprite_texture_uv_3d(gsGlobal, &tex, totalVertices, verts);
		
		gsKit_queue_exec(gsGlobal);
		gsKit_sync_flip(gsGlobal);
		gsKit_TexManager_nextFrame(gsGlobal);

		offset++;
	}

	free(verts);

	return 0;
}
