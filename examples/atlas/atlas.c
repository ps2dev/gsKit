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

struct runnerUV {
	uint32_t u0, v0;
	uint32_t u1, v1;
};

typedef struct runnerUV RunnerUV;

#define RUNNER_PER_LINE_SCREEN 16
#define RUNNER_PER_COLMN_SCREEN 11
#define TOTAL_RUNNERS_SCREEN (RUNNER_PER_LINE_SCREEN * RUNNER_PER_COLMN_SCREEN)

#define RUNNER_PER_LINE 8
#define RUNNER_PER_COLUMN 8
#define TOTAL_RUNNERS (RUNNER_PER_LINE * RUNNER_PER_COLUMN)

#define RUNNER_WIDTH 40
#define RUNNER_HEIGHT 40

static void getRunnerUV(uint32_t index, RunnerUV *runUV) {
	uint32_t line = index / RUNNER_PER_LINE;
	uint32_t col = index % RUNNER_PER_LINE;

	runUV->u0 = col * RUNNER_WIDTH;
	runUV->v0 = line * RUNNER_HEIGHT;
	runUV->u1 = runUV->u0 + RUNNER_WIDTH;
	runUV->v1 = runUV->v0 + RUNNER_HEIGHT;
}

int main(int argc, char *argv[])
{
	GSGLOBAL *gsGlobal;
	GSTEXTURE atlas;
	RunnerUV runUV;
	uint32_t index = 0;
	uint32_t totalVertices = 2 * TOTAL_RUNNERS_SCREEN;
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

	atlas.Delayed = 1;
	gsKit_texture_png(gsGlobal, &atlas, "runner_atlas.png");
	gsKit_TexManager_bind(gsGlobal, &atlas);
	printf("Atlas Height: %i\n",atlas.Height);
	printf("Atlas Width: %i\n",atlas.Width);

	printf("Atlas VRAM Range = 0x%X - 0x%X\n",atlas.Vram, atlas.Vram +gsKit_texture_size(atlas.Width, atlas.Height, atlas.PSM) - 1);

	gsKit_set_clamp(gsGlobal, GS_CMODE_CLAMP);
	
	GSPRIMUVPOINTFLAT *verts = (GSPRIMUVPOINTFLAT*)malloc(sizeof(GSPRIMUVPOINTFLAT) * totalVertices);
	for (int i = 0; i < TOTAL_RUNNERS_SCREEN; i++) {
		int line = i / RUNNER_PER_LINE_SCREEN;
		int col = i % RUNNER_PER_LINE_SCREEN;

		int x0 = col * RUNNER_WIDTH;
		int y0 = line * RUNNER_HEIGHT;
		int x1 = x0 + RUNNER_WIDTH;
		int y1 = y0 + RUNNER_HEIGHT;

		verts[i*2].xyz2 = vertex_to_XYZ2(gsGlobal, x0, y0, 0);
		verts[i*2+1].xyz2 = vertex_to_XYZ2(gsGlobal, x1, y1, 0);
	}

	while(1)
	{
		for (uint32_t i = 0; i < TOTAL_RUNNERS_SCREEN; i++) {
			uint32_t finalRunnerIndex = (index + i) % TOTAL_RUNNERS;
			getRunnerUV(finalRunnerIndex, &runUV);
			verts[i*2].uv = vertex_to_UV(&atlas, runUV.u0, runUV.v0);
			verts[i*2+1].uv = vertex_to_UV(&atlas, runUV.u1, runUV.v1);
		}

		gsKit_clear(gsGlobal, White);
		gskit_prim_list_sprite_texture_uv_flat_color(gsGlobal, &atlas, color, totalVertices, verts);
		
		gsKit_queue_exec(gsGlobal);
		gsKit_sync_flip(gsGlobal);
		gsKit_TexManager_nextFrame(gsGlobal);

		index = (index + 1) % TOTAL_RUNNERS;
	}

	free(verts);

	return 0;
}
