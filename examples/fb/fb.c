//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// fb.c - Example demonstrating advanced effects and CLUT usage.
// Special thanks to "gawd^" for this file! He wrote it, and it was used
// to help debug a really crappy CLUT/texturing bug!
//

#include <gsKit.h>
#include <dmaKit.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>

#include <gsToolkit.h>

#define USEBMP

static void generate_palette(GSTEXTURE *tex)
{
	int i;
	u32 *cptr;

	cptr = (u32 *)tex->Clut;

	for (i=0; i<128; i++)
	{
		/* 128 shades of red followed by 128 shades of yellow */
		cptr[i] = (i << 1);
		cptr[i+128] = ((i << 1) << 8) | 0xff;
	}
}
/*
static void generate_framex(GSTEXTURE *tex)
{
	int x, y;
	unsigned char *pixels, c;

	pixels = (unsigned char *)tex->Mem;
	for (y=0; y < tex->Height; y++)
	{
		c = (unsigned char)(255.0 * y / tex->Height);
		for (x=0; x < tex->Width; x++)
		{
			*pixels++ = c;
		}
	}
}
*/
static void generate_frame(GSTEXTURE *tex)
{
	int x, y;
	int stride;
	int a, b, c, d;
	int color;
	unsigned char *pixels;

	stride = tex->Width;
	pixels = (unsigned char *)tex->Mem;

	/* randominze last line */
	for (x=1; x<tex->Width; x++)
	{
		pixels[(tex->Height-1) * stride + x] = (rand() & 0xff);
	}

	/* soften */
	for (x=1; x<tex->Width-1; x++)
	{
		for (y=0; y < tex->Height-1; y++)
		{
			a = pixels[(y+1)*stride + x - 1];
			b = pixels[(y+1)*stride + x + 0];
			c = pixels[(y+1)*stride + x + 1];
			d = pixels[(y+2)*stride + x + 0];
			color = (a + b + c + d) / 4;

			if ((color > 0) && (color < 128))
			{
				/* fade */
				color--;
			}

			pixels[y*stride + x] = color;
		}
	}
}

int main(int argc, char *argv[])
{
#ifdef USEBMP
	GSTEXTURE backtex;
#endif
	GSTEXTURE fb;
	GSGLOBAL *gsGlobal;

	/* initialize dmaKit */
	dmaKit_init(D_CTRL_RELE_OFF,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC, D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);

    dmaKit_chan_init(DMA_CHANNEL_GIF);

	/* allocate GSGLOBAL structure */
	gsGlobal = gsKit_init_global();

	/* initialize screen */
	gsGlobal->PSM = GS_PSM_CT24;
	gsGlobal->ZBuffering = GS_SETTING_OFF; /* spare some vram */
	//  If we disable double buffering, we can't fill the frame fast enough.
	//  When this happens, we get a line through the top texture about 20% up
	//  from the bottom of the screen.
	// gsGlobal->DoubleBuffering = GS_SETTING_OFF; /* only one screen */
	gsKit_init_screen(gsGlobal);

	gsKit_mode_switch(gsGlobal, GS_PERSISTENT);

	fb.Width = 320;
	fb.Height = 200;
	fb.PSM = GS_PSM_T8;
	fb.ClutPSM = GS_PSM_CT32;

	fb.Mem = memalign(128, gsKit_texture_size_ee(fb.Width, fb.Height, fb.PSM));
	fb.Clut = memalign(128, gsKit_texture_size_ee(16, 16, fb.ClutPSM));

	fb.VramClut = gsKit_vram_alloc(gsGlobal, gsKit_texture_size(16, 16, fb.ClutPSM), GSKIT_ALLOC_USERBUFFER);
	fb.Vram = gsKit_vram_alloc(gsGlobal, gsKit_texture_size(fb.Width, fb.Height, fb.PSM), GSKIT_ALLOC_USERBUFFER);

	fb.Filter = GS_FILTER_LINEAR; /* enable bilinear filtering */

	gsKit_setup_tbw(&fb);

	generate_palette(&fb);

#ifdef USEBMP
	backtex.Delayed = 0;
	gsKit_texture_bmp(gsGlobal, &backtex, "host:bsdgirl.bmp");
#endif

	/* print out useless debug information */
	printf("CLUT Texture:\n");
	printf("\tHost  start: 0x%08x, end: 0x%08x\n", (unsigned)fb.Mem, (unsigned)(gsKit_texture_size_ee(fb.Width, fb.Height, fb.PSM) + fb.Mem));
	printf("\tLocal start: 0x%08x, end: 0x%08x\n", (unsigned)fb.Vram, (unsigned)(gsKit_texture_size(fb.Width, fb.Height, fb.PSM) + fb.Vram));
	printf("\tWidth - %d : Height - %d : TBW - %d : Page - %d : Block %d\n", fb.Width, fb.Height, fb.TBW, (fb.Vram / 8192), (fb.Vram / 256));
	printf("CLUT Pallete:\n");
	printf("\tHost  start: 0x%08x, end: 0x%08x\n", (unsigned)fb.Clut, (unsigned)(gsKit_texture_size_ee(16, 16, GS_PSM_CT32) + fb.Clut));
	printf("\tLocal start: 0x%08x, end: 0x%08x\n", (unsigned)fb.VramClut, (unsigned)(gsKit_texture_size(16, 16, GS_PSM_CT32) + fb.VramClut));
	printf("\tWidth - %d : Height - %d : TBW - %d : Page - %d : Block %d\n", 16, 16, 1, (fb.VramClut / 8192), (fb.VramClut / 256));
#ifdef USEBMP
	printf("BMP Texture:\n");
	printf("\tHost  start: 0x%08x, end: 0x%08x\n", (unsigned)backtex.Mem, (unsigned)(gsKit_texture_size_ee(backtex.Width, backtex.Height, backtex.PSM) + backtex.Mem));
	printf("\tLocal start: 0x%08x, end: 0x%08x\n", (unsigned)backtex.Vram, (unsigned)(gsKit_texture_size(backtex.Width, backtex.Height, backtex.PSM) + backtex.Vram));
	printf("\tWidth - %d : Height - %d : TBW - %d : Page - %d : Block %d\n", backtex.Width, backtex.Height, backtex.TBW, (backtex.Vram / 8192), (backtex.Vram / 256));
#endif
	printf("VRAM Alignment Check - Value of \"0\" is OKAY! Anything else is BAD!\n");
	printf("VRAM - CLUT Pallete - Start Address Aligned: %d\n", fb.VramClut % GS_VRAM_BLOCKSIZE_256);
	printf("VRAM - CLUT Texture - Start Address Aligned: %d\n", fb.Vram % GS_VRAM_BLOCKSIZE_256);
#ifdef USEBMP
	printf("VRAM - BMP Texture - Start Address Aligned: %d\n", backtex.Vram % GS_VRAM_BLOCKSIZE_256);
#endif

	/* clear buffer */
	gsKit_clear(gsGlobal, GS_SETREG_RGBAQ(0x00,0x00,0x00,0x00,0x00));

	/* render frame buffer */
	gsKit_prim_sprite_texture( gsGlobal,	&fb,
						0.0f, /* X1 */
						0.0f, /* Y1 */
						0.0f, /* U1 */
						0.0f, /* V1 */
						gsGlobal->Width, /* X2 */
						gsGlobal->Height, /* Y2 */
						fb.Width, /* U2 */
						fb.Height, /* V2*/
						1, /* Z */
						GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00) /* RGBAQ */
						);


#ifdef USEBMP
	gsKit_prim_sprite_texture(gsGlobal,	&backtex,
						(gsGlobal->Width /2) - (backtex.Width / 2), /* X1 */
						0.0f, /* Y1 */
						0.0f, /* U1 */
						0.0f, /* V1 */
						backtex.Width + ((gsGlobal->Width /2) - (backtex.Width / 2)), /* X2 */
						backtex.Height, /* Y2 */
						backtex.Width, /* U2 */
						backtex.Height, /* V2*/
						2, /* Z */
						GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00) /* RGBAQ */
						);
#endif

	while (1)
	{
		/* generate next frame */
		generate_frame(&fb);

		/* upload new frame buffer */
		gsKit_texture_upload(gsGlobal, &fb);

		/* execute render queue */
		gsKit_queue_exec(gsGlobal);

		/* vsync and flip buffer */
		gsKit_sync_flip(gsGlobal);
	}

	/* keep compilers happy (tm) */
	return 0;
}

