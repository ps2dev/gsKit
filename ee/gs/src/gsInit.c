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
// Parts taken from ooPo's ee-syscalls.txt
// http://www.oopo.net/consoledev/files/ee-syscalls.txt
//
// Parts taken from emoon's BreakPoint Demo Library
//

#include "gsKit.h"

void gsKit_init(unsigned int interlace, unsigned int mode, unsigned int field)
{
	__asm__ __volatile__("
		# SetGsCrt
		li  $3, 0x02;
		syscall;
		nop;");
}

void gsKit_init_screen(GSGLOBAL *gsGlobal)
{
	u64	*p_data;
	u64	*p_store;

	if(!gsGlobal->Setup)
	{
		gsGlobal->CurrentPointer = (-GS_VRAM_BLOCKSIZE)&(0+GS_VRAM_BLOCKSIZE-1);
	
		GS_RESET();

		__asm__("sync.p;" 
			"nop;");

		GsPutIMR(0x0000F700);

		gsKit_init(gsGlobal->Interlace, gsGlobal->Mode, gsGlobal->Field);

		gsGlobal->Setup = 1;
	}

	GS_SET_PMODE(0,		// Read Circuit 1
		     1,		// Read Circuit 2
		     0,		// Use ALP Register for Alpha Blending
		     1,		// Alpha Value of Read Circuit 2 for Output Selection
		     0,		// Blend Alpha with output of Read Circuit 2
		     0x80);	// Alpha Value = 1.0

	GS_SET_DISPFB1(0,			// Frame Buffer Base Pointer (Address/2048)
		       gsGlobal->Width / 64,	// Buffer Width (Address/64)
		       gsGlobal->PSM,		// Pixel Storage Format
		       0,			// Upper Left X in Buffer
		       0);			// Upper Left Y in Buffer

	GS_SET_DISPFB2(0,                       // Frame Buffer Base Pointer (Address/2048)
		       gsGlobal->Width / 64,    // Buffer Width (Address/64)
		       gsGlobal->PSM,           // Pixel Storage Format
		       0,                       // Upper Left X in Buffer
		       0);                      // Upper Left Y in Buffer

	GS_SET_DISPLAY1(656+gsGlobal->StartX,   // X position in the display area (in VCK unit
			35+gsGlobal->StartY,    // Y position in the display area (in Raster u
			gsGlobal->MagX,         // Horizontal Magnification
			gsGlobal->MagY,         // Vertical Magnification
			(gsGlobal->Width-1)*4,  // Display area width
			(gsGlobal->Height-1));  // Display area height

	GS_SET_DISPLAY2(656+gsGlobal->StartX,	// X position in the display area (in VCK units)
			35+gsGlobal->StartY,	// Y position in the display area (in Raster units)
			gsGlobal->MagX,		// Horizontal Magnification
			gsGlobal->MagY,		// Vertical Magnification
			(gsGlobal->Width-1)*4,	// Display area width
			(gsGlobal->Height-1));	// Display area height

	GS_SET_BGCOLOR(gsGlobal->BGColor->Red,		// Red
		       gsGlobal->BGColor->Green,	// Green
		       gsGlobal->BGColor->Blue);	// Blue

	gsGlobal->CurrentPointer = (-GS_VRAM_BLOCKSIZE)&(0+GS_VRAM_BLOCKSIZE-1);
	gsGlobal->ScreenBuffer[0] = gsKit_vram_alloc( gsGlobal, gsKit_texture_size(gsGlobal->Width, gsGlobal->Height, gsGlobal->PSM) ); // Context 1
	gsGlobal->ScreenBuffer[1] = gsKit_vram_alloc( gsGlobal, gsKit_texture_size(gsGlobal->Width, gsGlobal->Height, gsGlobal->PSM) ); // Context 2
	gsGlobal->ZBuffer = gsKit_vram_alloc( gsGlobal, gsKit_texture_size(gsGlobal->Width, gsGlobal->Height, gsGlobal->PSMZ) ); // Z Buffer

	p_data = p_store  = dmaKit_spr_alloc( 13*16 );

	*p_data++ = GIF_TAG( 12, 1, 0, 0, 0, 1 );
	*p_data++ = GIF_AD;

	*p_data++ = 1;
	*p_data++ = GS_PRMODECONT;
	
	*p_data++ = GS_SETREG_FRAME_1( gsGlobal->ScreenBuffer[0], gsGlobal->Width / 64, gsGlobal->PSM, 0 );
	*p_data++ = GS_FRAME_1;

	*p_data++ = GS_SETREG_XYOFFSET_1( gsGlobal->OffsetX << 4, gsGlobal->OffsetY << 4 );
	*p_data++ = GS_XYOFFSET_1;

	*p_data++ = GS_SETREG_SCISSOR_1( 0, gsGlobal->Width - 1, 0, gsGlobal->Height - 1 );
	*p_data++ = GS_SCISSOR_1;

	*p_data++ = GS_SETREG_ZBUF_1( gsGlobal->ZBuffer / 8192, gsGlobal->PSMZ, 0 );
	*p_data++ = GS_ZBUF_1;

	*p_data++ = GS_SETREG_TEST( gsGlobal->Test->ATE, gsGlobal->Test->ATST, 
				    gsGlobal->Test->AREF, gsGlobal->Test->AFAIL, 
				    gsGlobal->Test->DATE, gsGlobal->Test->DATM,
				    gsGlobal->Test->ZTE, gsGlobal->Test->ZTST );
	
	*p_data++ = GS_TEST_1;

	*p_data++ = GS_SETREG_COLCLAMP( 255 );
	*p_data++ = GS_COLCLAMP;

	*p_data++ = GS_SETREG_FRAME_1( gsGlobal->ScreenBuffer[1], gsGlobal->Width / 64, gsGlobal->PSM, 0 );
	*p_data++ = GS_FRAME_2;

	*p_data++ = GS_SETREG_XYOFFSET_1( gsGlobal->OffsetX << 4, gsGlobal->OffsetY << 4);
	*p_data++ = GS_XYOFFSET_2;

	*p_data++ = GS_SETREG_SCISSOR_1( 0, gsGlobal->Width - 1, 0, gsGlobal->Height - 1);
	*p_data++ = GS_SCISSOR_2;

	*p_data++ = GS_SETREG_ZBUF_1( gsGlobal->ZBuffer / 8192, gsGlobal->PSMZ, 0 );
	*p_data++ = GS_ZBUF_2;

	*p_data++ = GS_SETREG_TEST( gsGlobal->Test->ATE, gsGlobal->Test->ATST, 
				    gsGlobal->Test->AREF, gsGlobal->Test->AFAIL, 
				    gsGlobal->Test->DATE, gsGlobal->Test->DATM,
				    gsGlobal->Test->ZTE, gsGlobal->Test->ZTST );
	
	*p_data++ = GS_TEST_2;

	dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, 13 );
	gsKit_set_test(gsGlobal, GS_ZTEST_INIT);
	gsKit_set_clamp(gsGlobal, GS_CMODE_INIT);
	
}

GSGLOBAL *gsKit_init_global(u8 mode)
{
	
	GSGLOBAL *gsGlobal = calloc(1,sizeof(GSGLOBAL));
	gsGlobal->BGColor = calloc(1,sizeof(GSBGCOLOR));	
	gsGlobal->Test = calloc(1,sizeof(GSTEST));
	gsGlobal->Clamp = calloc(1,sizeof(GSCLAMP));

        /* Generic Values */
        gsGlobal->Interlace = GS_NONINTERLACED;
        gsGlobal->Field = GS_FRAME;
        gsGlobal->Setup = 0;
        gsGlobal->Aspect = GS_ASPECT_4_3;

	if(mode == GS_MODE_NTSC)
	{
	        gsGlobal->Mode = GS_MODE_NTSC;
	        gsGlobal->Width = 640;
	        gsGlobal->Height = 480;
	}
	else if(mode == GS_MODE_PAL)
	{
		gsGlobal->Mode = GS_MODE_PAL;
		gsGlobal->Width = 720;
		gsGlobal->Height = 576;
	}

	// TODO Add the rest of the mode values here!
		
        gsGlobal->OffsetX = 2048;
        gsGlobal->OffsetY = 2048;
        gsGlobal->StartX = 0;
        gsGlobal->StartY = 0;
        gsGlobal->MagX = 3;
        gsGlobal->MagY = 0;
        gsGlobal->PSM = GS_PSM_CT16;
        gsGlobal->PSMZ = GS_PSMZ_16;
        gsGlobal->ActiveBuffer = 1;
        gsGlobal->PrimFogEnable = 0;
        gsGlobal->PrimAAEnable = 0;
        gsGlobal->PrimAlphaEnable = 1;
        gsGlobal->PrimAlpha = 1;
        gsGlobal->PrimContext = 0;

        /* BGColor Register Values */
        gsGlobal->BGColor->Red = 0x00;
        gsGlobal->BGColor->Green = 0x00;
        gsGlobal->BGColor->Blue = 0x0;

        /* TEST Register Values */
        gsGlobal->Test->ATE = 0;
        gsGlobal->Test->ATST = 1;
        gsGlobal->Test->AREF = 0x80;
        gsGlobal->Test->AFAIL = 0;
        gsGlobal->Test->DATE = 0;
        gsGlobal->Test->DATM = 0;
        gsGlobal->Test->ZTE = 1;
        gsGlobal->Test->ZTST = 2;

	gsGlobal->Clamp->WMS = GS_CMODE_CLAMP;
	gsGlobal->Clamp->WMT = GS_CMODE_CLAMP;
        gsGlobal->Clamp->MINU = 0;
        gsGlobal->Clamp->MAXU = 0;
        gsGlobal->Clamp->MINV = 0;
        gsGlobal->Clamp->MAXV = 0;

	return gsGlobal;
}
