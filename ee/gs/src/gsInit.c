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

	gsGlobal->CurrentPointer = (-GS_VRAM_BLOCKSIZE)&(0+GS_VRAM_BLOCKSIZE-1);

	GS_RESET();

	__asm__("sync.p;" 
		"nop;");

	GsPutIMR(0x0000F700);

	gsKit_init(gsGlobal->Interlace, gsGlobal->Mode, gsGlobal->Field);

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
			3,                      // Horizontal Magnification
			0,                      // Vertical Magnification
			(gsGlobal->Width-1)*4,      // Display area width
			(gsGlobal->Height-1));      // Display area height

	GS_SET_DISPLAY2(656+gsGlobal->StartX,	// X position in the display area (in VCK units)
			35+gsGlobal->StartY,	// Y position in the display area (in Raster units)
			3,			// Horizontal Magnification
			0,			// Vertical Magnification
			(gsGlobal->Width-1)*4,	// Display area width
			(gsGlobal->Height-1));	// Display area height

	GS_SET_BGCOLOR(gsGlobal->BGColor.Red,		// Red
		       gsGlobal->BGColor.Green,	// Green
		       gsGlobal->BGColor.Blue);	// Blue

	gsGlobal->CurrentPointer = (-GS_VRAM_BLOCKSIZE)&(0+GS_VRAM_BLOCKSIZE-1);
	gsGlobal->ScreenBuffer[0] = gsKit_vram_alloc( gsGlobal, gsGlobal->Height*gsGlobal->Width*4 ); // Context 1
	gsGlobal->ScreenBuffer[1] = gsKit_vram_alloc( gsGlobal, gsGlobal->Height*gsGlobal->Width*4 ); // Context 2
	gsGlobal->ZBuffer = gsKit_vram_alloc( gsGlobal, gsGlobal->Height*gsGlobal->Width*4 ); // Z Buffer

	p_data = p_store  = dmaKit_spr_alloc( 13*16 );

	*p_data++ = GIF_TAG( 12, 1, 0, 0, 0, 1 );
	*p_data++ = GIF_AD;

	*p_data++ = 1;
	*p_data++ = GS_PRMODECONT;
	
	*p_data++ = GS_SETREG_FRAME_1( 0, gsGlobal->Width / 64, gsGlobal->PSM, 0 );
	*p_data++ = GS_FRAME_1;

	*p_data++ = GS_SETREG_XYOFFSET_1( gsGlobal->OffsetX << 4, gsGlobal->OffsetY << 4 );
	*p_data++ = GS_XYOFFSET_1;

	*p_data++ = GS_SETREG_SCISSOR_1( 0, gsGlobal->Width - 1, 0, gsGlobal->Height - 1 );
	*p_data++ = GS_SCISSOR_1;

	*p_data++ = GS_SETREG_ZBUF_1( gsGlobal->ZBuffer / 8192, gsGlobal->PSMZ, 0 );
	*p_data++ = GS_ZBUF_1;

	*p_data++ = GS_SETREG_TEST( gsGlobal->Test.ATE, gsGlobal->Test.ATST, 
				    gsGlobal->Test.AREF, gsGlobal->Test.AFAIL, 
				    gsGlobal->Test.DATE, gsGlobal->Test.DATM,
				    gsGlobal->Test.ZTE, gsGlobal->Test.ZTST );
	
	*p_data++ = GS_TEST_1;

	*p_data++ = GS_SETREG_COLCLAMP( 255 );
	*p_data++ = GS_COLCLAMP;

	*p_data++ = GS_SETREG_FRAME_1( 0, gsGlobal->Width / 64, gsGlobal->PSM, 0 );
	*p_data++ = GS_FRAME_2;

	*p_data++ = GS_SETREG_XYOFFSET_1( gsGlobal->OffsetX << 4, gsGlobal->OffsetY << 4);
	*p_data++ = GS_XYOFFSET_2;

	*p_data++ = GS_SETREG_SCISSOR_1( 0, gsGlobal->Width - 1, 0, gsGlobal->Height - 1);
	*p_data++ = GS_SCISSOR_2;

	*p_data++ = GS_SETREG_ZBUF_1( gsGlobal->ZBuffer / 8192, gsGlobal->PSMZ, 0 );
	*p_data++ = GS_ZBUF_2;

	*p_data++ = GS_SETREG_TEST( gsGlobal->Test.ATE, gsGlobal->Test.ATST, 
				    gsGlobal->Test.AREF, gsGlobal->Test.AFAIL, 
				    gsGlobal->Test.DATE, gsGlobal->Test.DATM,
				    gsGlobal->Test.ZTE, gsGlobal->Test.ZTST );
	
	*p_data++ = GS_TEST_2;

	dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, 13 );
}
