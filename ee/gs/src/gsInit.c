//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsInit.c - GS initialization and configuration routines.
//
// Parts taken from emoon's BreakPoint Demo Library
//

#include "gsKit.h"

#include <stdio.h>
#include <kernel.h>
#include <osd_config.h>

static u8 modelSupportsGetGsDxDyOffset;

short int gsKit_check_rom(void)
{
	static int default_signal = -1;
	char romname[15];

	if(default_signal < 0)
	{
		GetRomName((char *)romname);
		romname[14] = '\0';

		//ROMVER string format: VVVVRTYYYYMMDD
		default_signal = (romname[4] == 'E') ? GS_MODE_PAL : GS_MODE_NTSC;
		modelSupportsGetGsDxDyOffset = (20010608 < atoi(&romname[6]));
	}

	return default_signal;
}

void gsKit_set_buffer_attributes(GSGLOBAL *gsGlobal)
{
	int gs_DX, gs_DY, gs_DW, gs_DH;

	gsGlobal->StartXOffset = 0;
	gsGlobal->StartYOffset = 0;

	switch (gsGlobal->Mode) {
		case GS_MODE_NTSC:
			gsGlobal->StartX = 492;
			gsGlobal->StartY = 34;
			gsGlobal->DW = 2880;
			gsGlobal->DH = 480;
			break;
		case GS_MODE_PAL:
			gsGlobal->StartX = 520;
			gsGlobal->StartY = 40;
			gsGlobal->DW = 2880;
			gsGlobal->DH = 576;
			break;
		case GS_MODE_VGA_640_60:
			gsGlobal->StartX = 280;
			gsGlobal->StartY = 18;
			gsGlobal->DW = 1280;
			gsGlobal->DH = 480;
			break;
		case GS_MODE_VGA_640_72:
			gsGlobal->StartX = 330;
			gsGlobal->StartY = 18;
			gsGlobal->DW = 1280;
			gsGlobal->DH = 480;
			break;
		case GS_MODE_VGA_640_75:
			gsGlobal->StartX = 360;
			gsGlobal->StartY = 18;
			gsGlobal->DW = 1280;
			gsGlobal->DH = 480;
			break;
		case GS_MODE_VGA_640_85:
			gsGlobal->StartX = 260;
			gsGlobal->StartY = 18;
			gsGlobal->DW = 1280;
			gsGlobal->DH = 480;
			break;
		case GS_MODE_VGA_800_56:
			gsGlobal->StartX = 450;
			gsGlobal->StartY = 25;
			gsGlobal->DW = 1600;
			gsGlobal->DH = 600;
			break;
		case GS_MODE_VGA_800_60:
		case GS_MODE_VGA_800_72:
			gsGlobal->StartX = 465;
			gsGlobal->StartY = 25;
			gsGlobal->DW = 1600;
			gsGlobal->DH = 600;
			break;
		case GS_MODE_VGA_800_75:
			gsGlobal->StartX = 510;
			gsGlobal->StartY = 25;
			gsGlobal->DW = 1600;
			gsGlobal->DH = 600;
			break;
		case GS_MODE_VGA_800_85:
			gsGlobal->StartX = 500;
			gsGlobal->StartY = 25;
			gsGlobal->DW = 1600;
			gsGlobal->DH = 600;
			break;
		case GS_MODE_VGA_1024_60:
			gsGlobal->StartX = 580;
			gsGlobal->StartY = 30;
			gsGlobal->DW = 2048; // does this really need doubling? can't test
			gsGlobal->DH = 768;
			break;
		case GS_MODE_VGA_1024_70:
			gsGlobal->StartX = 266;
			gsGlobal->StartY = 30;
			gsGlobal->DW = 1024;
			gsGlobal->DH = 768;
			break;
		case GS_MODE_VGA_1024_75:
			gsGlobal->StartX = 260;
			gsGlobal->StartY = 30;
			gsGlobal->DW = 1024;
			gsGlobal->DH = 768;
			break;
		case GS_MODE_VGA_1024_85:
			gsGlobal->StartX = 290;
			gsGlobal->StartY = 30;
			gsGlobal->DW = 1024;
			gsGlobal->DH = 768;
			break;
		case GS_MODE_VGA_1280_60:
		case GS_MODE_VGA_1280_75:
			gsGlobal->StartX = 350;
			gsGlobal->StartY = 40;
			gsGlobal->DW = 1280;
			gsGlobal->DH = 1024;
			break;
		case GS_MODE_DTV_480P:
			gsGlobal->StartX = 232;
			gsGlobal->StartY = 35;
			gsGlobal->DW = 1440;
			gsGlobal->DH = 480; // though rare there are tv's that can handle an interlaced 480p source
			break;
		case GS_MODE_DTV_576P:
			gsGlobal->StartX = 255;
			gsGlobal->StartY = 44;
			gsGlobal->DW = 1440;
			gsGlobal->DH = 576;
			break;
		case GS_MODE_DTV_720P:
			gsGlobal->StartX = 306;
			gsGlobal->StartY = 24;
			gsGlobal->DW = 1280;
			gsGlobal->DH = 720;
			break;
		case GS_MODE_DTV_1080I:
			gsGlobal->StartX = 236;
			gsGlobal->StartY = 38;
			gsGlobal->DW = 1920;
			gsGlobal->DH = 1080;
			break;
	}

	gsGlobal->MagH = (gsGlobal->DW / gsGlobal->Width) - 1; // gsGlobal->DW should be a multiple of the screen width
	gsGlobal->MagV = (gsGlobal->DH / gsGlobal->Height) - 1; // gsGlobal->DH should be a multiple of the screen height

	// For other video modes, other than NTSC and PAL: if this model supports the GetGsDxDy syscall, get the board-specific offsets.
	if(gsGlobal->Mode != GS_MODE_NTSC && gsGlobal->Mode != GS_MODE_PAL)
	{
		gsKit_check_rom();
		if(modelSupportsGetGsDxDyOffset)
		{
			_GetGsDxDyOffset(gsGlobal->Mode, &gs_DX, &gs_DY, &gs_DW, &gs_DH);
			gsGlobal->StartX += gs_DX;
			gsGlobal->StartY += gs_DY;
		}
	}

	// Keep the framebuffer in the center of the screen
	gsGlobal->StartX += (gsGlobal->DW - ((gsGlobal->MagH + 1) * gsGlobal->Width )) / 2;
	gsGlobal->StartY += (gsGlobal->DH - ((gsGlobal->MagV + 1) * gsGlobal->Height)) / 2;

	if (gsGlobal->Interlace == GS_INTERLACED) {
		// Do not change odd/even start position in interlaced mode
		gsGlobal->StartY &= ~1;
	}

	// Calculate the actual display width and height
	gsGlobal->DW = (gsGlobal->MagH + 1) * gsGlobal->Width;
	gsGlobal->DH = (gsGlobal->MagV + 1) * gsGlobal->Height;

	if ((gsGlobal->Interlace == GS_INTERLACED) && (gsGlobal->Field == GS_FRAME)) {
		gsGlobal->MagV--;
	}
}

void gsKit_set_display_offset(GSGLOBAL *gsGlobal, int x, int y)
{
	gsGlobal->StartXOffset = x;
	gsGlobal->StartYOffset = y;

	if (gsGlobal->Interlace == GS_INTERLACED) {
		// Do not change odd/even start position in interlaced mode
		gsGlobal->StartYOffset &= ~1;
	}

	GS_SET_DISPLAY1(
			gsGlobal->StartX + gsGlobal->StartXOffset,	// X position in the display area (in VCK unit
			gsGlobal->StartY + gsGlobal->StartYOffset,	// Y position in the display area (in Raster u
			gsGlobal->MagH,			// Horizontal Magnification
			gsGlobal->MagV,			// Vertical Magnification
			gsGlobal->DW - 1,	// Display area width
			gsGlobal->DH - 1);		// Display area height

	GS_SET_DISPLAY2(
			gsGlobal->StartX + gsGlobal->StartXOffset,	// X position in the display area (in VCK units)
			gsGlobal->StartY + gsGlobal->StartYOffset,	// Y position in the display area (in Raster units)
			gsGlobal->MagH,			// Horizontal Magnification
			gsGlobal->MagV,			// Vertical Magnification
			gsGlobal->DW - 1,	// Display area width
			gsGlobal->DH - 1);		// Display area height
}

void gsKit_init_screen(GSGLOBAL *gsGlobal)
{
	u64	*p_data;
	u64	*p_store;
	int	size = 19;

	if((gsGlobal->Dithering == GS_SETTING_ON) &&
	   ((gsGlobal->PSM == GS_PSM_CT16) || (gsGlobal->PSM == GS_PSM_CT16S)))
		size++;

    gsKit_set_buffer_attributes(gsGlobal);

#ifdef DEBUG
    printf("Screen Mode:\n");
    printf("------------\n");
	printf("Width : %d\n", gsGlobal->Width);
	printf("Height: %d\n", gsGlobal->Height);
	printf("StartX: %d\n", gsGlobal->StartX);
	printf("StartY: %d\n", gsGlobal->StartY);
	printf("MagH  : %d\n", gsGlobal->MagH);
	printf("MagV  : %d\n", gsGlobal->MagV);
	printf("DW    : %d\n", gsGlobal->DW);
	printf("DH    : %d\n", gsGlobal->DH);
#endif

    GS_RESET();

    __asm__("sync.p; nop;");

    *GS_CSR = 0x00000000; // Clean CSR registers

    GsPutIMR(0x00007F00); // Masks all interrupts

	SetGsCrt(gsGlobal->Interlace, gsGlobal->Mode, gsGlobal->Field);

	// Fix 1080i frame mode
	if ((gsGlobal->Mode == GS_MODE_DTV_1080I) && (gsGlobal->Field == GS_FRAME))
		GS_SET_SMODE2(1, 1, 0);

	gsGlobal->FirstFrame = GS_SETTING_ON;

	if(gsGlobal->ZBuffering == GS_SETTING_OFF)
	{
		gsGlobal->Test->ZTE = GS_SETTING_ON;
		gsGlobal->Test->ZTST = 1;
	}

	DIntr(); // disable interrupts

	GS_SET_PMODE(	0,		// Read Circuit 1
			1,		// Read Circuit 2
			0,		// Use ALP Register for Alpha Blending
			1,		// Alpha Value of Read Circuit 2 for Output Selection
			0,		// Blend Alpha with output of Read Circuit 2
			0x80);		// Alpha Value = 1.0

	GS_SET_DISPFB1(	0,			// Frame Buffer Base Pointer (Address/2048)
			gsGlobal->Width / 64,	// Buffer Width (Address/64)
			gsGlobal->PSM,		// Pixel Storage Format
			0,			// Upper Left X in Buffer
			0);

	GS_SET_DISPFB2(	0,			// Frame Buffer Base Pointer (Address/2048)
			gsGlobal->Width / 64,	// Buffer Width (Address/64)
			gsGlobal->PSM,		// Pixel Storage Format
			0,			// Upper Left X in Buffer
			0);			// Upper Left Y in Buffer

	GS_SET_DISPLAY1(
			gsGlobal->StartX + gsGlobal->StartXOffset,	// X position in the display area (in VCK unit
			gsGlobal->StartY + gsGlobal->StartYOffset,	// Y position in the display area (in Raster u
			gsGlobal->MagH,			// Horizontal Magnification
			gsGlobal->MagV,			// Vertical Magnification
			gsGlobal->DW - 1,	// Display area width
			gsGlobal->DH - 1);		// Display area height

	GS_SET_DISPLAY2(
			gsGlobal->StartX + gsGlobal->StartXOffset,	// X position in the display area (in VCK units)
			gsGlobal->StartY + gsGlobal->StartYOffset,	// Y position in the display area (in Raster units)
			gsGlobal->MagH,			// Horizontal Magnification
			gsGlobal->MagV,			// Vertical Magnification
			gsGlobal->DW - 1,	// Display area width
			gsGlobal->DH - 1);		// Display area height

	GS_SET_BGCOLOR(	gsGlobal->BGColor->Red,		// Red
			gsGlobal->BGColor->Green,	// Green
			gsGlobal->BGColor->Blue);	// Blue

    EIntr(); //enable interrupts

    gsGlobal->CurrentPointer = 0; // reset vram pointer too

	// Context 1
	gsGlobal->ScreenBuffer[0] = gsKit_vram_alloc( gsGlobal, gsKit_texture_size(gsGlobal->Width, gsGlobal->Height, gsGlobal->PSM), GSKIT_ALLOC_SYSBUFFER );

	if(gsGlobal->DoubleBuffering == GS_SETTING_OFF)
	{
		gsGlobal->ScreenBuffer[1] = gsGlobal->ScreenBuffer[0];
	}
	else
		// Context 2
		gsGlobal->ScreenBuffer[1] = gsKit_vram_alloc( gsGlobal, gsKit_texture_size(gsGlobal->Width, gsGlobal->Height, gsGlobal->PSM), GSKIT_ALLOC_SYSBUFFER );


	if(gsGlobal->ZBuffering == GS_SETTING_ON)
		gsGlobal->ZBuffer = gsKit_vram_alloc( gsGlobal, gsKit_texture_size(gsGlobal->Width, gsGlobal->Height, gsGlobal->PSMZ), GSKIT_ALLOC_SYSBUFFER ); // Z Buffer

    gsGlobal->TexturePointer = gsGlobal->CurrentPointer; // first useable address for textures

	p_data = p_store = (u64 *)gsGlobal->dma_misc;

	*p_data++ = GIF_TAG( size - 1, 1, 0, 0, 0, 1 );
	*p_data++ = GIF_AD;

	*p_data++ = 1;
	*p_data++ = GS_PRMODECONT;

	*p_data++ = GS_SETREG_FRAME_1( gsGlobal->ScreenBuffer[0] / 8192, gsGlobal->Width / 64, gsGlobal->PSM, 0 );
	*p_data++ = GS_FRAME_1;

	*p_data++ = GS_SETREG_XYOFFSET_1( gsGlobal->OffsetX,
					  gsGlobal->OffsetY);
	*p_data++ = GS_XYOFFSET_1;

	*p_data++ = GS_SETREG_SCISSOR_1( 0, gsGlobal->Width - 1, 0, gsGlobal->Height - 1 );
	*p_data++ = GS_SCISSOR_1;

	*p_data++ = GS_SETREG_TEST( gsGlobal->Test->ATE, gsGlobal->Test->ATST,
				gsGlobal->Test->AREF, gsGlobal->Test->AFAIL,
				gsGlobal->Test->DATE, gsGlobal->Test->DATM,
				gsGlobal->Test->ZTE, gsGlobal->Test->ZTST );

	*p_data++ = GS_TEST_1;

	*p_data++ = GS_SETREG_CLAMP(gsGlobal->Clamp->WMS, gsGlobal->Clamp->WMT,
				gsGlobal->Clamp->MINU, gsGlobal->Clamp->MAXU,
				gsGlobal->Clamp->MINV, gsGlobal->Clamp->MAXV);

	*p_data++ = GS_CLAMP_1;

	if(gsGlobal->ZBuffering == GS_SETTING_ON)
	{
	    if((gsGlobal->PSM == GS_PSM_CT16) && (gsGlobal->PSMZ != GS_PSMZ_16))
            gsGlobal->PSMZ = GS_PSMZ_16; // seems only non-S 16-bit z depth works with this mode
        if((gsGlobal->PSM != GS_PSM_CT16) && (gsGlobal->PSMZ == GS_PSMZ_16))
            gsGlobal->PSMZ = GS_PSMZ_16S; // other depths don't seem to work with 16-bit non-S z depth

		*p_data++ = GS_SETREG_ZBUF_1( gsGlobal->ZBuffer / 8192, gsGlobal->PSMZ, 0 );
		*p_data++ = GS_ZBUF_1;
	}
	if(gsGlobal->ZBuffering == GS_SETTING_OFF)
	{
		*p_data++ = GS_SETREG_ZBUF_1( 0, gsGlobal->PSM, 1 );
		*p_data++ = GS_ZBUF_1;
	}

	*p_data++ = GS_SETREG_COLCLAMP( 255 );
	*p_data++ = GS_COLCLAMP;

	*p_data++ = GS_SETREG_FRAME_1( gsGlobal->ScreenBuffer[1] / 8192, gsGlobal->Width / 64, gsGlobal->PSM, 0 );
	*p_data++ = GS_FRAME_2;

	*p_data++ = GS_SETREG_XYOFFSET_1( gsGlobal->OffsetX,
					  gsGlobal->OffsetY);
	*p_data++ = GS_XYOFFSET_2;

	*p_data++ = GS_SETREG_SCISSOR_1( 0, gsGlobal->Width - 1, 0, gsGlobal->Height - 1);
	*p_data++ = GS_SCISSOR_2;

	*p_data++ = GS_SETREG_TEST( gsGlobal->Test->ATE, gsGlobal->Test->ATST,
				gsGlobal->Test->AREF, gsGlobal->Test->AFAIL,
				gsGlobal->Test->DATE, gsGlobal->Test->DATM,
				gsGlobal->Test->ZTE, gsGlobal->Test->ZTST );

	*p_data++ = GS_TEST_2;

	*p_data++ = GS_SETREG_CLAMP(gsGlobal->Clamp->WMS, gsGlobal->Clamp->WMT,
				gsGlobal->Clamp->MINU, gsGlobal->Clamp->MAXU,
				gsGlobal->Clamp->MINV, gsGlobal->Clamp->MAXV);

	*p_data++ = GS_CLAMP_2;

	if(gsGlobal->ZBuffering == GS_SETTING_ON)
	{
		*p_data++ = GS_SETREG_ZBUF_1( gsGlobal->ZBuffer / 8192, gsGlobal->PSMZ, 0 );
		*p_data++ = GS_ZBUF_2;
	}
	if(gsGlobal->ZBuffering == GS_SETTING_OFF)
	{
		*p_data++ = GS_SETREG_ZBUF_1( 0, gsGlobal->PSM, 1 );
		*p_data++ = GS_ZBUF_2;
	}

	*p_data++ = GS_BLEND_BACK2FRONT;
	*p_data++ = GS_ALPHA_1;

	*p_data++ = GS_BLEND_BACK2FRONT;
	*p_data++ = GS_ALPHA_2;

	*p_data++ = GS_SETREG_DIMX(gsGlobal->DitherMatrix[0],gsGlobal->DitherMatrix[1],
                gsGlobal->DitherMatrix[2],gsGlobal->DitherMatrix[3],gsGlobal->DitherMatrix[4],
                gsGlobal->DitherMatrix[5],gsGlobal->DitherMatrix[6],gsGlobal->DitherMatrix[7],
                gsGlobal->DitherMatrix[8],gsGlobal->DitherMatrix[9],gsGlobal->DitherMatrix[10],
                gsGlobal->DitherMatrix[11],gsGlobal->DitherMatrix[12],gsGlobal->DitherMatrix[13],
                gsGlobal->DitherMatrix[14],gsGlobal->DitherMatrix[15]); // 4x4 dither matrix

	*p_data++ = GS_DIMX;

	*p_data++ = GS_SETREG_TEXA(0x80, 0, 0x80);
	*p_data++ = GS_TEXA;

	if((gsGlobal->Dithering == GS_SETTING_ON) && ((gsGlobal->PSM == GS_PSM_CT16) || (gsGlobal->PSM == GS_PSM_CT16S))) {
        *p_data++ = 1;
        *p_data++ = GS_DTHE;
	}

	dmaKit_send_ucab(DMA_CHANNEL_GIF, p_store, size);
	dmaKit_wait_fast();

    // do a single frame here to get rid whatever needs to be done during the first frame

	gsKit_clear(gsGlobal, GS_SETREG_RGBAQ(0x00,0x00,0x00,0x00,0x00)); // clear the screen

	gsKit_queue_exec(gsGlobal);

	gsKit_sync_flip(gsGlobal);

	gsKit_queue_reset(gsGlobal->Os_Queue);
}

GSGLOBAL *gsKit_init_global_custom(int Os_AllocSize, int Per_AllocSize)
{
    //s8 dither_matrix[16] = {-4,2,-3,3,0,-2,1,-1,-3,3,-4,2,1,-1,0,-2};
    s8 dither_matrix[16] = {4,2,5,3,0,6,1,7,5,3,4,2,1,7,0,6}; //different matrix
    int i = 0;

	GSGLOBAL *gsGlobal = calloc(1,sizeof(GSGLOBAL));
	gsGlobal->BGColor = calloc(1,sizeof(GSBGCOLOR));
	gsGlobal->Test = calloc(1,sizeof(GSTEST));
	gsGlobal->Clamp = calloc(1,sizeof(GSCLAMP));
	gsGlobal->Os_Queue = calloc(1,sizeof(GSQUEUE));
	gsGlobal->Per_Queue = calloc(1,sizeof(GSQUEUE));
	gsGlobal->dma_misc = (u64 *)((u32)memalign(64, 512) | 0x30000000);

	FlushCache(0);

	/* Generic Values */
	if(configGetTvScreenType() == 2) gsGlobal->Aspect = GS_ASPECT_16_9;
    else
    gsGlobal->Aspect = GS_ASPECT_4_3;

    gsGlobal->PSM = GS_PSM_CT24;
    gsGlobal->PSMZ = GS_PSMZ_32;

    gsGlobal->Dithering = GS_SETTING_OFF;
    gsGlobal->DoubleBuffering = GS_SETTING_ON;
    gsGlobal->ZBuffering = GS_SETTING_ON;

    // Setup a mode automatically
    gsGlobal->Mode = gsKit_check_rom();
    gsGlobal->Interlace = GS_INTERLACED;
	gsGlobal->Field = GS_FIELD;
    gsGlobal->Width = 640;

    if(gsGlobal->Mode == GS_MODE_PAL) gsGlobal->Height = 512;
    else gsGlobal->Height = 448;

    gsGlobal->CurrentPointer = 0;

	gsGlobal->DrawOrder = GS_PER_OS;

	gsGlobal->EvenOrOdd = 0;

	gsGlobal->Os_AllocSize = Os_AllocSize;
	gsKit_queue_init(gsGlobal, gsGlobal->Os_Queue, GS_ONESHOT, Os_AllocSize);

	gsGlobal->Per_AllocSize = Per_AllocSize;
	gsKit_queue_init(gsGlobal, gsGlobal->Per_Queue, GS_PERSISTENT, Per_AllocSize);

	gsGlobal->CurQueue = gsGlobal->Os_Queue;

	gsGlobal->OffsetX = (int)(2048.0f * 16.0f);
	gsGlobal->OffsetY = (int)(2048.0f * 16.0f);
	gsGlobal->ActiveBuffer = 1;
    gsGlobal->LockBuffer = GS_SETTING_OFF;
	gsGlobal->PrimFogEnable = GS_SETTING_OFF;
	gsGlobal->PrimAAEnable = GS_SETTING_OFF;
	gsGlobal->PrimAlphaEnable = GS_SETTING_OFF;
	gsGlobal->PrimAlpha = GS_BLEND_BACK2FRONT;
	gsGlobal->PrimContext = 0;
	gsGlobal->FirstFrame = GS_SETTING_ON;

	for(i = 0; i < 15; i++) {
	    gsGlobal->DitherMatrix[i] = dither_matrix[i];
	}

	/* BGColor Register Values */
	gsGlobal->BGColor->Red = 0x00;
	gsGlobal->BGColor->Green = 0x00;
	gsGlobal->BGColor->Blue = 0x00;

	/* TEST Register Values */
	gsGlobal->Test->ATE = GS_SETTING_OFF;
	gsGlobal->Test->ATST = GS_SETTING_ON;
	gsGlobal->Test->AREF = 0x80;
	gsGlobal->Test->AFAIL = 0;
	gsGlobal->Test->DATE = GS_SETTING_OFF;
	gsGlobal->Test->DATM = 0;
	gsGlobal->Test->ZTE = GS_SETTING_ON;
	gsGlobal->Test->ZTST = 2;

	gsGlobal->Clamp->WMS = GS_CMODE_CLAMP;
	gsGlobal->Clamp->WMT = GS_CMODE_CLAMP;
	gsGlobal->Clamp->MINU = 0;
	gsGlobal->Clamp->MAXU = 0;
	gsGlobal->Clamp->MINV = 0;
	gsGlobal->Clamp->MAXV = 0;

	return gsGlobal;
}

void gsKit_deinit_global(GSGLOBAL *gsGlobal)
{
	gsKit_queue_free(gsGlobal, gsGlobal->Per_Queue);
	gsKit_queue_free(gsGlobal, gsGlobal->Os_Queue);

    gsGlobal->dma_misc = (u64 *)((u32)gsGlobal->dma_misc ^ 0x30000000);

    free(gsGlobal->dma_misc);
    free(gsGlobal->Per_Queue);
    free(gsGlobal->Os_Queue);
    free(gsGlobal->Clamp);
    free(gsGlobal->Test);
    free(gsGlobal->BGColor);
    free(gsGlobal);
}
