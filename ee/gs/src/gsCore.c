//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsCore.c - C/Assembler implimentation of all GS instructions.
//
// Parts taken from emoon's BreakPoint Demo Library
//

#include "gsKit.h"

u32 gsKit_vram_alloc(struct gsGlobals gsGlobal, int size)
{	
	gsGlobal.CurrentPointer = (-GS_VRAM_BLOCKSIZE)&(0+GS_VRAM_BLOCKSIZE-1);
	size = (-GS_VRAM_BLOCKSIZE)&(size+GS_VRAM_BLOCKSIZE-1);
	gsGlobal.CurrentPointer += size;
	
	return gsGlobal.CurrentPointer - size;
}

GSGLOBAL gsKit_sync_flip(GSGLOBAL gsGlobal)
{
	gsKit_vsync();
  
	GS_SET_DISPFB2( gsGlobal.ScreenBuffer[gsGlobal.ActiveBuffer & 1] / 8192,
		gsGlobal.Width / 64, gsGlobal.PSM, 0, 0 );
  
	gsGlobal.ActiveBuffer ^= 1;

	gsKit_setactive( gsGlobal );

	return gsGlobal;
}



GSGLOBAL gsKit_setactive(GSGLOBAL gsGlobal)
{
	u64 *p_data;
	u64 *p_store;

	p_data = p_store = dmaKit_spr_alloc( (3+2)*16 );
	
	*p_data++ = GIF_TAG( 2+2, 1, 0, 0, 0, 1 );
	*p_data++ = GIF_AD;
	
	// Context 1

	*p_data++ = GS_SETREG_SCISSOR_1( 0, gsGlobal.Width-1, 0, gsGlobal.Height - 1 );
	*p_data++ = GS_SCISSOR_1;

	*p_data++ = GS_SETREG_FRAME_1( gsGlobal.ScreenBuffer[gsGlobal.ActiveBuffer & 1] / 8192,
                                 gsGlobal.Width / 64, gsGlobal.PSM, 0 );
	*p_data++ = GS_FRAME_1;

	// Context 2
	
	*p_data++ = GS_SETREG_SCISSOR_1( 0, gsGlobal.Width-1, 0, gsGlobal.Height - 1 );
	*p_data++ = GS_SCISSOR_2;

	*p_data++ = GS_SETREG_FRAME_1( gsGlobal.ScreenBuffer[gsGlobal.ActiveBuffer & 1] / 8192,
                                 gsGlobal.Width / 64, gsGlobal.PSM, 0 );
	*p_data++ = GS_FRAME_2;

	dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, 3+2 );

	return gsGlobal;
}

void gsKit_vsync(void)
{
	*CSR = *CSR & 8;
	while(!(*CSR & 8));
}

void gsKit_clear(GSGLOBAL gsGlobal, int Color)
{
	gsKit_prim_sprite(gsGlobal, 0, 0, gsGlobal.Width, gsGlobal.Height, 0, Color);
}
