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

u32 gsKit_vram_alloc(GSGLOBAL *gsGlobal, u32 size)
{	
	size = (-GS_VRAM_BLOCKSIZE)&(size+GS_VRAM_BLOCKSIZE-1);
	gsGlobal->CurrentPointer += size;

	printf("Current Vram Point %i\n",gsGlobal->CurrentPointer - size);
	
	if(gsGlobal->CurrentPointer >= 4194304)
	{
		printf("ERROR: Not enough VRAM for this allocation!\n");
		return -1;
	}
	else
		return gsGlobal->CurrentPointer - size;
}

void gsKit_sync_flip(GSGLOBAL *gsGlobal)
{
	gsKit_vsync();
  
	GS_SET_DISPFB2( gsGlobal->ScreenBuffer[gsGlobal->ActiveBuffer & 1] / 8192,
		gsGlobal->Width / 64, gsGlobal->PSM, 0, 0 );
  
	gsGlobal->ActiveBuffer ^= 1;
	gsGlobal->PrimContext ^= 1;

	gsGlobal->EvenOrOdd=((GSREG*)CSR)->FIELD;

	gsKit_setactive(gsGlobal);
}



void gsKit_setactive(GSGLOBAL *gsGlobal)
{
	u64 *p_data;
	u64 *p_store;

	p_data = p_store = dmaKit_spr_alloc( 5*16 );
	
	*p_data++ = GIF_TAG( 4, 1, 0, 0, 0, 1 );
	*p_data++ = GIF_AD;
	
	// Context 1

	*p_data++ = GS_SETREG_SCISSOR_1( 0, gsGlobal->Width - 1, 0, gsGlobal->Height - 1 );
	*p_data++ = GS_SCISSOR_1;

	*p_data++ = GS_SETREG_FRAME_1( gsGlobal->ScreenBuffer[gsGlobal->ActiveBuffer & 1] / 8192,
                                 gsGlobal->Width / 64, gsGlobal->PSM, 0 );
	*p_data++ = GS_FRAME_1;

	// Context 2
	
	*p_data++ = GS_SETREG_SCISSOR_1( 0, gsGlobal->Width - 1, 0, gsGlobal->Height - 1 );
	*p_data++ = GS_SCISSOR_2;

	*p_data++ = GS_SETREG_FRAME_1( gsGlobal->ScreenBuffer[gsGlobal->ActiveBuffer & 1] / 8192,
                                 gsGlobal->Width / 64, gsGlobal->PSM, 0 );
	*p_data++ = GS_FRAME_2;

	dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, 5 );
}

void gsKit_vsync(void)
{
	*CSR = *CSR & 8;
	while(!(*CSR & 8));
}

void gsKit_clear(GSGLOBAL *gsGlobal, u64 color)
{
	u8 PrevZState = gsGlobal->Test->ZTST;
	gsKit_set_test(gsGlobal, GS_ZTEST_OFF);
	gsKit_prim_sprite(gsGlobal, 0, 0, gsGlobal->Width, gsGlobal->Height, 0, color);
	gsGlobal->Test->ZTST = PrevZState;
	gsKit_set_test(gsGlobal, 0);
}

void gsKit_set_test(GSGLOBAL *gsGlobal, u8 Preset)
{
	u64 *p_data;
	u64 *p_store;

	if(Preset == GS_ZTEST_OFF)
		gsGlobal->Test->ZTST = 1;
	else if (Preset == GS_ZTEST_ON)
		gsGlobal->Test->ZTST = 2;
	else if (Preset == GS_ATEST_OFF)
		gsGlobal->Test->ATE = 0;
	else if (Preset == GS_ATEST_ON)
		gsGlobal->Test->ATE = 1;
	else if (Preset == GS_D_ATEST_OFF)
		gsGlobal->Test->DATE = 0;
	else if (Preset == GS_D_ATEST_ON)
		gsGlobal->Test->DATE = 1;

	p_data = p_store = dmaKit_spr_alloc(2*16);

	*p_data++ = GIF_TAG( 1, 1, 0, 0, 0, 1 );
	*p_data++ = GIF_AD;

	*p_data++ = GS_SETREG_TEST( gsGlobal->Test->ATE, gsGlobal->Test->ATST,
				    gsGlobal->Test->AREF, gsGlobal->Test->AFAIL, 
				    gsGlobal->Test->DATE, gsGlobal->Test->DATM, 
				    gsGlobal->Test->ZTE, gsGlobal->Test->ZTST );

	*p_data++ = GS_TEST_1+gsGlobal->PrimContext;

	dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, 2);
}

void gsKit_set_clamp(GSGLOBAL *gsGlobal, u8 Preset)
{
	u64 *p_data;
	u64 *p_store;
	
	if(Preset == GS_CMODE_REPEAT)
	{
		gsGlobal->Clamp->WMS = GS_CMODE_REPEAT;
		gsGlobal->Clamp->WMT = GS_CMODE_REPEAT;
	}
	else if(Preset == GS_CMODE_CLAMP)
	{
                gsGlobal->Clamp->WMS = GS_CMODE_CLAMP;
                gsGlobal->Clamp->WMT = GS_CMODE_CLAMP;
	}
	else if(Preset == GS_CMODE_REGION_CLAMP)
	{
                gsGlobal->Clamp->WMS = GS_CMODE_REGION_CLAMP;
                gsGlobal->Clamp->WMT = GS_CMODE_REGION_CLAMP;
	}
	else if(Preset == GS_CMODE_REGION_REPEAT)
	{
                gsGlobal->Clamp->WMS = GS_CMODE_REGION_REPEAT;
                gsGlobal->Clamp->WMT = GS_CMODE_REGION_REPEAT;
	}

	p_data = p_store = dmaKit_spr_alloc(2*16);	

        *p_data++ = GIF_TAG( 1, 1, 0, 0, 0, 1 );
        *p_data++ = GIF_AD;

	*p_data++ = GS_SETREG_CLAMP(gsGlobal->Clamp->WMS, gsGlobal->Clamp->WMT, gsGlobal->Clamp->MINU, 
				    gsGlobal->Clamp->MAXU, gsGlobal->Clamp->MINV, gsGlobal->Clamp->MAXV);

	*p_data++ = GS_CLAMP_1+gsGlobal->PrimContext;

	dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, 2);
}
