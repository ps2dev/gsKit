//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// renderTexture.c - Example demonstrating render to texture operation.
// First we are rendering a triangle to a texture, 
// then we are rendering the texture to the screen.
//

#include <stdio.h>
#include <malloc.h>

#include <gsKit.h>
#include <dmaKit.h>
#include <gsToolkit.h>
#include <gsInline.h>
#include <gsTexture.h>

#define TEXTURE_WIDTH 256
#define TEXTURE_HEIGHT 256

int main(int argc, char *argv[])
{
	GSGLOBAL *gsGlobal;
	GSTEXTURE renderTexture;
	uint32_t i, j, offset;
	uint32_t totalVertices = 3;
	uint32_t textureTotalVertices = 2;
	uint64_t White = GS_SETREG_RGBAQ(0xFF, 0xFF, 0xFF, 0x00, 0x00);
	gs_rgbaq color = color_to_RGBAQ(0x80, 0x80, 0x80, 0x80, 0);
	u32 render_target_ptr;

	gsGlobal = gsKit_init_global();

	gsGlobal->PSM = GS_PSM_CT24;
	gsGlobal->PSMZ = GS_PSMZ_16S;

	dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
				D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);

	gsKit_init_screen(gsGlobal);
	gsKit_mode_switch(gsGlobal, GS_ONESHOT);

	renderTexture.Width = TEXTURE_WIDTH;
	renderTexture.Height = TEXTURE_HEIGHT;
	renderTexture.PSM = GS_PSM_CT16;
	renderTexture.Mem = 0; // NOT NEEDED
	renderTexture.Vram = gsKit_vram_alloc(gsGlobal, gsKit_texture_size(renderTexture.Width, renderTexture.Height, renderTexture.PSM), GSKIT_ALLOC_USERBUFFER);

	gsKit_set_clamp(gsGlobal, GS_CMODE_CLAMP);

	GSPRIMPOINT *verts = (GSPRIMPOINT *)malloc(sizeof(GSPRIMPOINT) * totalVertices);
	verts[0].xyz2 = vertex_to_XYZ2(gsGlobal, 0, 0, 0);
	verts[0].rgbaq = color_to_RGBAQ(0xFF, 0x00, 0x00, 0xFF, 0);

	verts[1].xyz2 = vertex_to_XYZ2(gsGlobal, renderTexture.Width, 0, 0);
	verts[1].rgbaq = color_to_RGBAQ(0x00, 0xFF, 0x00, 0xFF, 0);

	verts[2].xyz2 = vertex_to_XYZ2(gsGlobal, renderTexture.Width / 2, renderTexture.Height, 0);
	verts[2].rgbaq = color_to_RGBAQ(0x00, 0x00, 0xFF, 0xFF, 0);

	GSPRIMUVPOINTFLAT *textureVertex = (GSPRIMUVPOINTFLAT *)malloc(sizeof(GSPRIMUVPOINTFLAT) * textureTotalVertices);
	textureVertex[0].xyz2 = vertex_to_XYZ2(gsGlobal, 0, 0, 0);
	textureVertex[0].uv = vertex_to_UV(&renderTexture, 0, 0);

	textureVertex[1].xyz2 = vertex_to_XYZ2(gsGlobal, gsGlobal->Width, gsGlobal->Height, 0);
	textureVertex[1].uv = vertex_to_UV(&renderTexture, renderTexture.Width, renderTexture.Height);

	while (1)
	{
		gsKit_clear(gsGlobal, White);
		
		// set render target to the texture
		gsKit_renderToTexture(gsGlobal, &renderTexture);

		gsKit_prim_list_triangle_gouraud_3d(gsGlobal, totalVertices, verts);

		// set render target back to the screen
		gsKit_renderToScreen(gsGlobal);

		gskit_prim_list_sprite_texture_uv_flat_color(gsGlobal, &renderTexture, color, textureTotalVertices, textureVertex);

		gsKit_queue_exec(gsGlobal);
		gsKit_sync_flip(gsGlobal);
	}

	free(verts);

	return 0;
}
