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

	p_data = p_store = dmaKit_spr_alloc( 5*16 );
	
	*p_data++ = GIF_TAG( 4, 1, 0, 0, 0, 1 );
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

	dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, 5 );

	return gsGlobal;
}

void gsKit_vsync(void)
{
	*CSR = *CSR & 8;
	while(!(*CSR & 8));
}

void gsKit_clear(GSGLOBAL gsGlobal, u64 color)
{
	u8 PrevZState = gsGlobal.Test.ZTE;
	gsKit_set_test(gsGlobal, GS_ZTEST_OFF);
	gsKit_prim_sprite(gsGlobal, 0, 0, gsGlobal.Width, gsGlobal.Height, 0, color);
	gsGlobal.Test.ZTE = PrevZState;
	gsKit_set_test(gsGlobal, 0);
}

int gsKit_set_test(GSGLOBAL gsGlobal, u8 Preset)
{
	u64 *p_data;
	u64 *p_store;

	if(Preset == GS_ZTEST_OFF)
	{
		gsGlobal.Test.ZTE = 0;
	}
	else if (Preset == GS_ZTEST_ON)
	{
		gsGlobal.Test.ZTE = 1;
	}
	else if (Preset == GS_ATEST_OFF)
	{
		gsGlobal.Test.ATE = 0;
	}
	else if (Preset == GS_ATEST_ON)
	{
		gsGlobal.Test.ATE = 1;
	}
	else if (Preset == GS_D_ATEST_OFF)
	{
		gsGlobal.Test.DATE = 0;
	}
	else if (Preset == GS_D_ATEST_ON)
	{
		gsGlobal.Test.DATE = 1;
	}

	p_data = p_store = dmaKit_spr_alloc(2*16);

	*p_data++ = GIF_TAG( 1, 1, 0, 0, 0, 1 );
	*p_data++ = GIF_AD;

	*p_data++ = GS_SETREG_TEST( gsGlobal.Test.ATE, gsGlobal.Test.ATST,
				    gsGlobal.Test.AREF, gsGlobal.Test.AFAIL, 
				    gsGlobal.Test.DATE, gsGlobal.Test.DATM, 
				    gsGlobal.Test.ZTE, gsGlobal.Test.ZTST );

	if( gsGlobal.PrimContext == 0 )
		*p_data++ = GS_TEST_1;
	else if(gsGlobal.PrimContext == 1 )
		*p_data++ = GS_TEST_2;

	dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, 2);

	return 0;
}
