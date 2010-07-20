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

short int gsKit_detect_signal(void)
{
	char romname[14];

	GetRomName((char *)romname);

	if (romname[4] == 'E') {
		return GS_MODE_PAL;
	}
	else {
		return GS_MODE_NTSC;
	}
}

void gsKit_set_buffer_attributes(GSGLOBAL *gsGlobal)
{
	switch (gsGlobal->Mode) {
		case GS_MODE_NTSC:
			gsGlobal->StartX = 652;
			gsGlobal->StartY = 26;
			gsGlobal->DW = 2560;
			gsGlobal->DH = 224;
			break;
		case GS_MODE_PAL:
			gsGlobal->StartX = 680;
			gsGlobal->StartY = 37;
			gsGlobal->DW = 2560;
			gsGlobal->DH = 256;
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
		case GS_MODE_DTV_720P:
			gsGlobal->StartX = 420;
			gsGlobal->StartY = 40;
			gsGlobal->DW = 1280;
			gsGlobal->DH = 720;
			break;
		case GS_MODE_DTV_1080I:
			gsGlobal->StartX = 300;
			gsGlobal->StartY = 120;
			gsGlobal->DW = 1920;
			gsGlobal->DH = 540;
			break;
	}

	if ((gsGlobal->Interlace == GS_INTERLACED) && (gsGlobal->Field == GS_FIELD)) {
		gsGlobal->DH *= 2;
		gsGlobal->StartY = (gsGlobal->StartY - 1) * 2;
	}

	gsGlobal->MagH = (gsGlobal->DW / gsGlobal->Width) - 1; // gsGlobal->DW should be a multiple of the screen width
	gsGlobal->MagV = (gsGlobal->DH / gsGlobal->Height) - 1; // gsGlobal->DH should be a multiple of the screen height

}

void gsKit_init_screen(GSGLOBAL *gsGlobal)
{
	u64	*p_data;
	u64	*p_store;
	int	size = 18;

	if((gsGlobal->Dithering == GS_SETTING_ON) && 
	   ((gsGlobal->PSM == GS_PSM_CT16) || (gsGlobal->PSM == GS_PSM_CT16S)))
		size = 19;

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

    GsPutIMR(0x0000F700); // Unmasks all of the GS interrupts

	SetGsCrt(gsGlobal->Interlace, gsGlobal->Mode, gsGlobal->Field);

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

	GS_SET_DISPLAY1(gsGlobal->StartX,		// X position in the display area (in VCK unit
			gsGlobal->StartY,		// Y position in the display area (in Raster u
			gsGlobal->MagH,			// Horizontal Magnification
			gsGlobal->MagV,			// Vertical Magnification
			gsGlobal->DW - 1,	// Display area width
			gsGlobal->DH - 1);		// Display area height

	GS_SET_DISPLAY2(gsGlobal->StartX,		// X position in the display area (in VCK units)
			gsGlobal->StartY,		// Y position in the display area (in Raster units)
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

	(u32)p_data = (u32)p_store = gsGlobal->dma_misc;

	*p_data++ = GIF_TAG( size - 1, 1, 0, 0, 0, 1 );
	*p_data++ = GIF_AD;

	*p_data++ = 1;
	*p_data++ = GS_PRMODECONT;

	*p_data++ = GS_SETREG_FRAME_1( gsGlobal->ScreenBuffer[0], gsGlobal->Width / 64, gsGlobal->PSM, 0 );
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
		*p_data++ = GS_SETREG_ZBUF_1( NULL, gsGlobal->PSM, 1 );
		*p_data++ = GS_ZBUF_1;
	}

	*p_data++ = GS_SETREG_COLCLAMP( 255 );
	*p_data++ = GS_COLCLAMP;

	*p_data++ = GS_SETREG_FRAME_1( gsGlobal->ScreenBuffer[1], gsGlobal->Width / 64, gsGlobal->PSM, 0 );
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
		*p_data++ = GS_SETREG_ZBUF_1( NULL, gsGlobal->PSM, 1 );
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
    s8 dither_matrix[16] = {-4,2,-3,3,0,-2,1,-1,-3,3,-4,2,1,-1,0,-2};
    //s8 dither_matrix[16] = {4,2,5,3,0,6,1,7,5,3,4,2,1,7,0,6}; //different matrix
    int i = 0;

	GSGLOBAL *gsGlobal = calloc(1,sizeof(GSGLOBAL));
	gsGlobal->BGColor = calloc(1,sizeof(GSBGCOLOR));
	gsGlobal->Test = calloc(1,sizeof(GSTEST));
	gsGlobal->Clamp = calloc(1,sizeof(GSCLAMP));
	gsGlobal->Os_Queue = calloc(1,sizeof(GSQUEUE));
	gsGlobal->Per_Queue = calloc(1,sizeof(GSQUEUE));
	(u32)gsGlobal->dma_misc = ((u32)memalign(64, 512) | 0x30000000);

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
    gsGlobal->Mode = gsKit_detect_signal();
    gsGlobal->Interlace = GS_INTERLACED;
	gsGlobal->Field = GS_FIELD;
    gsGlobal->Width = 640;

    if(gsGlobal->Mode == GS_MODE_PAL) gsGlobal->Height = 512;
    else gsGlobal->Height = 448;

    gsGlobal->CurrentPointer = 0;

	gsGlobal->DrawOrder = GS_PER_OS;

	gsGlobal->EvenOrOdd = 0;

	gsGlobal->Os_AllocSize = Os_AllocSize;
	(u32)gsGlobal->Os_Queue->dma_tag = (u32)gsGlobal->Os_Queue->pool[0] = ((u32)memalign(64, Os_AllocSize) | 0x30000000);
	(u32)gsGlobal->Os_Queue->pool[1] = ((u32)memalign(64, Os_AllocSize) | 0x30000000);
	(u32)gsGlobal->Os_Queue->pool_cur = ((u32)gsGlobal->Os_Queue->pool[0] + 16);
	(u32)gsGlobal->Os_Queue->pool_max[0] = ((u32)gsGlobal->Os_Queue->pool[0] + Os_AllocSize);
	(u32)gsGlobal->Os_Queue->pool_max[1] = ((u32)gsGlobal->Os_Queue->pool[1] + Os_AllocSize);
	gsGlobal->Os_Queue->dbuf = 0;
	gsGlobal->Os_Queue->tag_size = 0;
	gsGlobal->Os_Queue->last_tag = gsGlobal->Os_Queue->pool_cur;
	gsGlobal->Os_Queue->last_type = GIF_RESERVED;
	gsGlobal->Os_Queue->mode = GS_ONESHOT;

	gsGlobal->Per_AllocSize = Per_AllocSize;
	(u32)gsGlobal->Per_Queue->dma_tag = (u32)gsGlobal->Per_Queue->pool[0] = ((u32)memalign(64, Per_AllocSize) | 0x30000000);
	(u32)gsGlobal->Per_Queue->pool_cur = ((u32)gsGlobal->Per_Queue->pool[0] + 16);
	(u32)gsGlobal->Per_Queue->pool_max[0] = ((u32)gsGlobal->Per_Queue->pool[0] + Per_AllocSize);
	gsGlobal->Per_Queue->dbuf = 0;
	gsGlobal->Per_Queue->tag_size = 0;
	gsGlobal->Per_Queue->last_tag = gsGlobal->Per_Queue->pool_cur;
	gsGlobal->Per_Queue->last_type = GIF_RESERVED;
	gsGlobal->Per_Queue->mode = GS_PERSISTENT;

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
    (u32)gsGlobal->Per_Queue->pool[0] ^= 0x30000000;
    (u32)gsGlobal->Os_Queue->pool[1] ^= 0x30000000;
    (u32)gsGlobal->Os_Queue->pool[0] ^= 0x30000000;
    (u32)gsGlobal->dma_misc ^= 0x30000000;

    free(gsGlobal->Per_Queue->pool[0]);
    free(gsGlobal->Os_Queue->pool[1]);
    free(gsGlobal->Os_Queue->pool[0]);
    free(gsGlobal->dma_misc);
    free(gsGlobal->Per_Queue);
    free(gsGlobal->Os_Queue);
    free(gsGlobal->Clamp);
    free(gsGlobal->Test);
    free(gsGlobal->BGColor);
    free(gsGlobal);
}
