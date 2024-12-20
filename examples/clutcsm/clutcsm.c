//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// textures.c - Example demonstrating how the 2 different CLUT storage modes work.
// CSMT1 and CSM1 are the two different CLUT storage modes. CSM1 is the default
// mode, and is the mode that is used which requires to have the content swizzled
// before uploading it to the GS. However, CSM2 is a mode that allows you to
// upload the CLUT data directly to the GS, without having to swizzle it first.
// Additionally it allows you to upload a list of CLUTs, and then switch between
// them using the GS_SETCLUT command.
//
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

#define USING_CSM1 0

#if USING_CSM1
#define TEXTURE_CLUT_WIDTH 16
#define TEXTURE_CLUT_HEIGHT 16
#else
#define TEXTURE_CLUT_WIDTH 256
#define TEXTURE_CLUT_HEIGHT 1
#endif

int main(int argc, char *argv[])
{
	GSGLOBAL *gsGlobal;
	GSTEXTURE tex;
	uint32_t i, j, offset;
	uint32_t totalVertices = 2;
	uint64_t White = GS_SETREG_RGBAQ(0xFF, 0xFF, 0xFF, 0x00, 0x00);
	gs_rgbaq color = color_to_RGBAQ(0x80, 0x80, 0x80, 0x80, 0);

	gsGlobal = gsKit_init_global();

	gsGlobal->PSM = GS_PSM_CT24;
	gsGlobal->PSMZ = GS_PSMZ_16S;

	dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
				D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);

	gsKit_init_screen(gsGlobal);
	gsKit_mode_switch(gsGlobal, GS_ONESHOT);

	tex.Width = TEXTURE_WIDTH;
	tex.Height = TEXTURE_HEIGHT;
	tex.PSM = GS_PSM_T8;
	tex.ClutPSM = GS_PSM_CT16;
	tex.Clut = memalign(128, gsKit_texture_size_ee(CLUT_SIZE, 1, tex.ClutPSM));
	tex.Mem = memalign(128, gsKit_texture_size_ee(tex.Width, tex.Height, tex.PSM));
	tex.Vram = gsKit_vram_alloc(gsGlobal, gsKit_texture_size(tex.Width, tex.Height, GS_PSM_T8), GSKIT_ALLOC_USERBUFFER);
	tex.VramClut = gsKit_vram_alloc(gsGlobal, gsKit_texture_size(TEXTURE_CLUT_WIDTH, TEXTURE_CLUT_HEIGHT, GS_PSM_CT16), GSKIT_ALLOC_USERBUFFER);
#if USING_CSM1
	tex.ClutStorageMode = GS_CLUT_STORAGE_CSM1;
#else
	tex.ClutStorageMode = GS_CLUT_STORAGE_CSM2;
#endif

	printf("Tex VRAM Range = 0x%X - 0x%X\n", tex.Vram, tex.Vram + gsKit_texture_size(tex.Width, tex.Height, tex.PSM) - 1);

	gsKit_set_clamp(gsGlobal, GS_CMODE_CLAMP);

	uint16_t colors[4] = {
		0xEFFF,
		0x001F,
		0x03E0,
		0x7C00,
	};

	// fill the clut, each 16 positions are the same color
	uint16_t *clut = (uint16_t *)tex.Clut;
	for (i = 0; i < CLUT_SIZE; i += 16)
	{
		for (j = 0; j < 16; ++j)
		{
			clut[i + j] = colors[(i / 16) & 0x3];
		}
	}
#if USING_CSM1
	// Swap the texture to have the CSM1 requirements
	for (i = 0; i < CLUT_SIZE; i++)
	{
		if ((i & 0x18) == 8)
		{
			uint16_t tmp = clut[i];
			clut[i] = clut[i + 8];
			clut[i + 8] = tmp;
		}
	}
#endif

	// print the clut, 16 colors per line
	for (i = 0; i < CLUT_SIZE; i += 16)
	{
		for (j = 0; j < 16; ++j)
		{
			printf("%04X ", clut[i + j]);
		}
		printf("\n");
	}

	// initialize texture, each 16 horizontal pixels are the same color, so let's point to the right color
	uint8_t *tex256 = (uint8_t *)tex.Mem;
	for (i = 0; i < TEXTURE_HEIGHT; ++i)
	{
		for (j = 0; j < TEXTURE_WIDTH; ++j)
		{
			tex256[i * TEXTURE_WIDTH + j] = j & 0xFF;
		}
	}

	gsKit_texture_send((u32 *)tex.Mem, tex.Width, tex.Height, tex.Vram, tex.PSM, 1, GS_CLUT_TEXTURE);
	gsKit_texture_send((u32 *)tex.Clut, TEXTURE_CLUT_WIDTH, TEXTURE_CLUT_HEIGHT, tex.VramClut, tex.ClutPSM, 1, GS_CLUT_PALLETE);

	GSPRIMUVPOINTFLAT *verts = (GSPRIMUVPOINTFLAT *)malloc(sizeof(GSPRIMUVPOINTFLAT) * totalVertices);
	verts[0].xyz2 = vertex_to_XYZ2(gsGlobal, 0, 0, 0);
	verts[0].uv = vertex_to_UV(&tex, 0, 0);

	verts[1].xyz2 = vertex_to_XYZ2(gsGlobal, gsGlobal->Width, gsGlobal->Height, 0);
	verts[1].uv = vertex_to_UV(&tex, tex.Width, tex.Height);

	offset = 0;

	gs_texclut texclut = postion_to_TEXCLUT(4, 0, 0);

	while (1)
	{
		gsKit_clear(gsGlobal, White);
#if !USING_CSM1
		gsKit_set_texclut(gsGlobal, texclut);
#endif
		gskit_prim_list_sprite_texture_uv_flat_color(gsGlobal, &tex, color, totalVertices, verts);

		gsKit_queue_exec(gsGlobal);
		gsKit_sync_flip(gsGlobal);

		offset++;
	}

	free(verts);

	return 0;
}
