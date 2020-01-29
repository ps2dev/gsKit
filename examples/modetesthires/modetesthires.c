/*
 * How to use:
 *   L1/R1:      Cycle through all video modes
 *
 * Use ps2client to see the debug output, for example:
 *   Mode: 720p 1280x720 GS_Frame offset: 0, 0 memory: 576KiB
 */

#include <stdio.h>
#include <malloc.h>
#include <gsKit.h>
#include <gsHires.h>
#include <dmaKit.h>
#include "libpad.h"

extern int port, slot;
extern int pad_init();
u32 old_pad = 0;

struct SMode {
	const char * sMode;
	s16 Mode;
	s16 Interlace;
	s16 Field;
	int MaxWidth;
	int MaxHeight;
	int Width;
	int Height;
	int VCK;
	int XOff;
	int YOff;
	int iPassCount;
};

struct SMode modes[] = {
	{ "720p", GS_MODE_DTV_720P,  GS_NONINTERLACED, GS_FRAME, 1280,  720, 1280,  720, 0, 0, 0, 3},
	{"1080i", GS_MODE_DTV_1080I, GS_INTERLACED,    GS_FRAME, 1920, 1080, 1920, 1080, 0, 0, 0, 3},
};
int iCurrentMode = 0;
struct SMode * pCurrentMode = &modes[0];
int iModeChange = 1;

#define TEXTURE_WIDTH (320)
#define TEXTURE_HEIGHT (240)

unsigned int __attribute__((aligned(16))) tex256[TEXTURE_WIDTH*TEXTURE_HEIGHT];

static void prepareTexture(void) {
	// initialize texture
	unsigned int i,j;

	for (j = 0; j < TEXTURE_HEIGHT; ++j)
	{
		for (i = 0; i < TEXTURE_WIDTH; ++i)
		{
			tex256[i + j * TEXTURE_WIDTH] = j * i;
		}
	}
}

static void set_texture(GSTEXTURE *texture, const void *frame,
      int width, int height, int PSM, int filter)
{
   texture->Width = width;
   texture->Height = height;
   texture->PSM = PSM;
   texture->Filter = filter;
   texture->Mem = (void *)frame;
}

static void print_mode(GSGLOBAL *gsGlobal)
{
	printf("Mode: %s %dx%d %s offset: %d, %d memory: %dKiB\n",
		pCurrentMode->sMode,
		gsGlobal->Width,
		gsGlobal->Height,
		pCurrentMode->Field == GS_FRAME ? "GS_FRAME" : "GS_FIELD",
		pCurrentMode->XOff,
		pCurrentMode->YOff,
		gsGlobal->CurrentPointer / 1024);
}

static void get_pad(GSGLOBAL *gsGlobal)
{
	struct padButtonStatus buttons;
	u32 paddata;
	u32 new_pad;
	int ret;

	do {
		ret=padGetState(port, slot);
	} while((ret != PAD_STATE_STABLE) && (ret != PAD_STATE_FINDCTP1));

	ret = padRead(port, slot, &buttons);

	if (ret != 0) {
		paddata = 0xffff ^ buttons.btns;

		new_pad = paddata & ~old_pad;
		old_pad = paddata;

		// Change mode
		if(new_pad & PAD_R1) {
			iCurrentMode++;
			if (iCurrentMode > 1)
				iCurrentMode = 0;
			iModeChange = 1;
		}
		if(new_pad & PAD_L1) {
			iCurrentMode--;
			if (iCurrentMode < 0)
				iCurrentMode = 1;
			iModeChange = 1;
		}
	}
}

int main(int argc, char *argv[])
{
	u64 TexCol = GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00);
	GSGLOBAL *gsGlobal = NULL;
	GSTEXTURE *texture = calloc(1, sizeof(*texture));

	pad_init();

	// Load textures
	prepareTexture();
	set_texture(texture, tex256, TEXTURE_WIDTH, TEXTURE_HEIGHT, GS_PSM_CT32, GS_FILTER_LINEAR);

	dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    	D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);
	
	while(1) {
		if (iModeChange != 0) {
			iModeChange = 0;
			pCurrentMode = &modes[iCurrentMode];

			if(gsGlobal!= NULL) {
				gsKit_hires_deinit_global(gsGlobal);
			}
			gsGlobal = gsKit_hires_init_global();

			gsGlobal->PSM = GS_PSM_CT16S;
			gsGlobal->PSMZ = GS_PSMZ_16S;
			gsGlobal->Mode = pCurrentMode->Mode;
			gsGlobal->Interlace = pCurrentMode->Interlace;
			gsGlobal->Field = pCurrentMode->Field;
			gsGlobal->Width  = pCurrentMode->Width;
			gsGlobal->Dithering = GS_SETTING_ON;
			gsGlobal->DoubleBuffering = GS_SETTING_ON;
			gsGlobal->ZBuffering = GS_SETTING_ON;
			if ((pCurrentMode->Interlace == GS_INTERLACED) && (pCurrentMode->Field == GS_FRAME))
				gsGlobal->Height = pCurrentMode->Height/2;
			else
				gsGlobal->Height = pCurrentMode->Height;

			gsKit_hires_init_screen(gsGlobal, pCurrentMode->iPassCount);
			gsKit_set_display_offset(gsGlobal, pCurrentMode->XOff * pCurrentMode->VCK, pCurrentMode->YOff);

			print_mode(gsGlobal);

			printf("VRAM used: %dKiB\n", gsGlobal->CurrentPointer / 1024);
			printf("VRAM free: %dKiB\n", 4096 - (gsGlobal->CurrentPointer / 1024));
		}

		gsKit_TexManager_bind(gsGlobal, texture);

		gsKit_prim_sprite_texture(gsGlobal, texture,
                              0, //X1
                              0,  // Y1
                              0,  // U1
                              0,  // V1
                              gsGlobal->Width, // X2
                              gsGlobal->Height, // Y2
                              texture->Width, // U2
                              texture->Height, // V2
                              0,
                              TexCol);

		gsKit_hires_sync(gsGlobal);
		gsKit_hires_flip(gsGlobal);
		gsKit_TexManager_nextFrame(gsGlobal);
		get_pad(gsGlobal);
	}

	gsKit_hires_deinit_global(gsGlobal);

	return 0;
}
