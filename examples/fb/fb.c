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
#include <malloc.h>
#include <math.h>

#define USEBMP

/** clut lookup table */
static const short clut_xlut[256] =
{
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
	0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
	0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
	0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
	0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
	0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
	0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
	0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
	0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
	0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
	0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
	0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
	0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
	0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
	0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
	0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
	0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
	0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
	0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
	0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

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
	GSTEXTURE fb, backtex;
	GSGLOBAL *gsGlobal;

	/* initialize dmaKit */
	dmaKit_init(D_CTRL_RELE_OFF,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC, D_CTRL_STD_OFF, D_CTRL_RCYC_8);

        dmaKit_chan_init(DMA_CHANNEL_GIF);
        dmaKit_chan_init(DMA_CHANNEL_FROMSPR);
        dmaKit_chan_init(DMA_CHANNEL_TOSPR);

	/* allocate GSGLOBAL structure */
	gsGlobal = gsKit_init_global(GS_MODE_NTSC);

	/* initialize screen */
	gsGlobal->PSM = GS_PSM_CT24;
	gsGlobal->ZBuffering = GS_SETTING_OFF; /* spare some vram */
//	If we disable double buffering, we can't fill the frame fast enough.
//	When this happens, we get a line through the top texture about 20% up
//	from the bottom of the screen.
//	gsGlobal->DoubleBuffering = GS_SETTING_OFF; /* only one screen */
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

	generate_palette(&fb);

#ifdef USEBMP
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

                /* vsync and flip buffer */
                gsKit_sync_flip(gsGlobal);

		/* execute render queue */
		gsKit_queue_exec(gsGlobal);
	}

	/* keep compilers happy (tm) */
	return 0;
}

