/*
 * How to use:
 *   L1/R1:      Cycle through all video modes
 *   R3:         Change X/Y offset of screen
 *   X:          Switch between FIELD and FRAME mode
 *   LEFT/RIGHT: Change width in units of 64 pixels
 *   UP/DOWN:    Change height in units of 2 pixels
 *
 * Use ps2client to see the debug output, for example:
 *   Mode: 480i 576x452 GS_FIELD offset: -6, 0 memory: 576KiB
 */

#include <stdio.h>

#include <gsKit.h>
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
};

struct SMode modes[] = {
	// NTSC
	{ "480i", GS_MODE_NTSC,      GS_INTERLACED,    GS_FIELD,  704,  480,  704,  452, 4, 0, 0},
	{ "480p", GS_MODE_DTV_480P,  GS_NONINTERLACED, GS_FRAME,  704,  480,  704,  452, 2, 0, 0},
	// PAL
	{ "576i", GS_MODE_PAL,       GS_INTERLACED,    GS_FIELD,  704,  576,  704,  536, 4, 0, 0},
	{ "576p", GS_MODE_DTV_576P,  GS_NONINTERLACED, GS_FRAME,  704,  576,  704,  536, 2, 0, 0},
	// HDTV
	{ "720p", GS_MODE_DTV_720P,  GS_NONINTERLACED, GS_FRAME, 1280,  720, 1280,  698, 1, 0, 0},
	{"1080i", GS_MODE_DTV_1080I, GS_INTERLACED,    GS_FIELD, 1920, 1080, 1920, 1080, 1, 0, 0},
};
int iCurrentMode = 0;
struct SMode * pCurrentMode = &modes[0];
int iModeChange = 1;

void
print_mode(GSGLOBAL *gsGlobal)
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

void
render(GSGLOBAL *gsGlobal)
{
	const float fBorder1 =  4.0f;
	const float fBorder2 = 16.0f;
	const u64 clBorder1 = GS_SETREG_RGBAQ(255,0,0,0,0); // Red
	const u64 clBorder2 = GS_SETREG_RGBAQ(255,255,255,0,0); // White
	const u64 clBackground = GS_SETREG_RGBAQ(64,128,255,0,0); // Light blue
	float fYFactor = 1.0f;

	gsKit_queue_reset(gsGlobal->Per_Queue);

	gsKit_mode_switch(gsGlobal, GS_PERSISTENT);
	gsKit_clear(gsGlobal, clBorder1);

	if ((pCurrentMode->Interlace == GS_INTERLACED) && (pCurrentMode->Field == GS_FRAME))
		fYFactor = 0.5f;

	gsKit_prim_quad(gsGlobal, fBorder1, fBorder1*fYFactor,
				  fBorder1, gsGlobal->Height-fBorder1*fYFactor,
				  gsGlobal->Width-fBorder1, fBorder1*fYFactor,
				  gsGlobal->Width-fBorder1, gsGlobal->Height-fBorder1*fYFactor,
				  1, clBorder2);

	gsKit_prim_quad(gsGlobal, fBorder2, fBorder2*fYFactor,
				  fBorder2, gsGlobal->Height-fBorder2*fYFactor,
				  gsGlobal->Width-fBorder2, fBorder2*fYFactor,
				  gsGlobal->Width-fBorder2, gsGlobal->Height-fBorder2*fYFactor,
				  1, clBackground);

	gsKit_queue_exec(gsGlobal);
}

void
get_pad(GSGLOBAL *gsGlobal)
{
	struct padButtonStatus buttons;
	u32 paddata;
	u32 new_pad;
	int ret;
	float fXOff;
	float fYOff;
	static float fXOffAccum = 0.0f;
	static float fYOffAccum = 0.0f;

	do {
		ret=padGetState(port, slot);
	} while((ret != PAD_STATE_STABLE) && (ret != PAD_STATE_FINDCTP1));

	ret = padRead(port, slot, &buttons);

	if (ret != 0) {
		paddata = 0xffff ^ buttons.btns;

		new_pad = paddata & ~old_pad;
		old_pad = paddata;

		fXOff = (buttons.rjoy_h - 128.0f) / 128.0f;
		if ((fXOff < -0.5f) || (fXOff > 0.5f)) {
			//printf("%f\n", fXOff);
			fXOffAccum += fXOff/5.0f;
			if ((fXOffAccum < -1.0f) || (fXOffAccum > 1.0f)) {
				int iOff = fXOffAccum;
				pCurrentMode->XOff += iOff;
				fXOffAccum -= iOff;
				gsKit_set_display_offset(gsGlobal, pCurrentMode->XOff * pCurrentMode->VCK, pCurrentMode->YOff);
				print_mode(gsGlobal);
			}
		}

		fYOff = (buttons.rjoy_v - 128.0f) / 128.0f;
		if ((fYOff < -0.5f) || (fYOff > 0.5f)) {
			//printf("%f\n", fYOff);
			fYOffAccum += fYOff/5.0f;
			if ((fYOffAccum < -1.0f) || (fYOffAccum > 1.0f)) {
				int iOff = fYOffAccum;
				pCurrentMode->YOff += iOff;
				fYOffAccum -= iOff;
				gsKit_set_display_offset(gsGlobal, pCurrentMode->XOff * pCurrentMode->VCK, pCurrentMode->YOff);
				print_mode(gsGlobal);
			}
		}

		// Change width
		if(new_pad & PAD_LEFT) {
			if (pCurrentMode->Width > 64) {
				pCurrentMode->Width -= 64;
				iModeChange = 1;
			}
		}
		if(new_pad & PAD_RIGHT) {
			if (pCurrentMode->Width < pCurrentMode->MaxWidth) {
				pCurrentMode->Width += 64;
				iModeChange = 1;
			}
		}

		// Change height
		if(new_pad & PAD_DOWN) {
			if (pCurrentMode->Height > 64) {
				pCurrentMode->Height -= 2;
				iModeChange = 1;
			}
		}
		if(new_pad & PAD_UP) {
			if (pCurrentMode->Height < pCurrentMode->MaxHeight) {
				pCurrentMode->Height += 2;
				iModeChange = 1;
			}
		}

		// Switch between FIELD and FRAME mode
		if(new_pad & PAD_CROSS) {
			if (pCurrentMode->Interlace == GS_INTERLACED) {
				pCurrentMode->Field = (pCurrentMode->Field == GS_FRAME) ? GS_FIELD : GS_FRAME;
				iModeChange = 1;
			}
		}

		// Change mode
		if(new_pad & PAD_R1) {
			iCurrentMode++;
			if (iCurrentMode > 5)
				iCurrentMode = 0;
			iModeChange = 1;
		}
		if(new_pad & PAD_L1) {
			iCurrentMode--;
			if (iCurrentMode < 0)
				iCurrentMode = 5;
			iModeChange = 1;
		}
	}
}

int main(int argc, char *argv[])
{
	GSGLOBAL *gsGlobal = gsKit_init_global();

	pad_init();

	dmaKit_init(D_CTRL_RELE_OFF,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);
	dmaKit_chan_init(DMA_CHANNEL_GIF);

	gsGlobal->DoubleBuffering = GS_SETTING_OFF;
	gsGlobal->ZBuffering = GS_SETTING_OFF;

	while(1) {
		if (iModeChange != 0) {
			iModeChange = 0;
			pCurrentMode = &modes[iCurrentMode];

			gsGlobal->PSM = GS_PSM_CT16;
			gsGlobal->PSMZ = GS_PSMZ_16;
			gsGlobal->Mode = pCurrentMode->Mode;
			gsGlobal->Interlace = pCurrentMode->Interlace;
			gsGlobal->Field = pCurrentMode->Field;
			gsGlobal->Width  = pCurrentMode->Width;
			if ((pCurrentMode->Interlace == GS_INTERLACED) && (pCurrentMode->Field == GS_FRAME))
				gsGlobal->Height = pCurrentMode->Height/2;
			else
				gsGlobal->Height = pCurrentMode->Height;

			gsKit_vram_clear(gsGlobal);
			gsKit_init_screen(gsGlobal);
			gsKit_set_display_offset(gsGlobal, pCurrentMode->XOff * pCurrentMode->VCK, pCurrentMode->YOff);

			print_mode(gsGlobal);

			render(gsGlobal);
		}

		gsKit_sync_flip(gsGlobal);
		get_pad(gsGlobal);
	}

	return 0;
}
