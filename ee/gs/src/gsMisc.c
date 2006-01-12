//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsMisc.c - Miscellaneous Helper Routines
//

#include "gsKit.h"

int gsKit_scale(GSGLOBAL *gsGlobal, u8 axis, float vertex)
{
	int result = 0;

	if(axis == GS_AXIS_X || axis == GS_MAP_U || axis == GS_MAP_V)
	{			
		result = (int)(vertex * 16.0f);
		result += gsGlobal->OffsetX << 4;
	}
	else if(axis == GS_AXIS_Y)
	{
		if(gsGlobal->Field == GS_FRAME)
		{
			vertex = vertex / 2;
			if(!gsGlobal->EvenOrOdd)
				vertex += 0.5;
		}

		result = (int)(vertex * 16.0f);
		result += gsGlobal->OffsetY << 4;
	}
	else if(axis == GS_AXIS_Z)
	{
		result = (int)(vertex * 16.0f);
	}

	return result;

}

void gsKit_vram_dump(GSGLOBAL *gsGlobal, char *Path, u32 StartAddr, u32 EndAddr)
{
#if 0
	u64* p_store;
	u64* p_data;
	u32* p_mem;
	int packets;
	int remain;
	int qwc;
	int Size;
	
	printf("THIS IS NOT DONE YET!\n");
	
	return 0;
	
	StartAddr = (-GS_VRAM_BLOCKSIZE)&(StartAddr+GS_VRAM_BLOCKSIZE-1);
	EndAddr = (-GS_VRAM_BLOCKSIZE)&(EndAddr+GS_VRAM_BLOCKSIZE-1);
	
	Size = EndAddr - StartAddr;
	
	qwc = Size / 16;
	if( Size % 16 )
	{
#ifdef DEBUG
		printf("Uneven division of quad word count from VRAM alloc. Rounding up QWC.\n");
#endif
		qwc++;
	}

	packets = qwc / DMA_MAX_SIZE;
	remain  = qwc % DMA_MAX_SIZE;

	p_store = p_data = dmaKit_spr_alloc( 7 * 16 );

	FlushCache(0);

	// DMA DATA
	*p_data++ = DMA_TAG( 6, 0, DMA_CNT, 0, 0, 0 );
	*p_data++ = 0;

	*p_data++ = GIF_TAG( 5, 1, 0, 0, 0, 1 );
	*p_data++ = GIF_AD;

	*p_data++ = GS_SETREG_BITBLTBUF(StartAddr / 64, (4095.9375 / 64), 0, 0, 0, 0);
	*p_data++ = GS_BITBLTBUF;

	*p_data++ = GS_SETREG_TRXPOS(0, 0, 0, 0, 0);
	*p_data++ = GS_TRXPOS;

	*p_data++ = GS_SETREG_TRXREG(4095.9375, 4095.9375);
	*p_data++ = GS_TRXREG;
	
	*p_data++ = 0;
	*p_data++ = GS_FINISH;

	*p_data++ = GS_SETREG_TRXDIR(1);
	*p_data++ = GS_TRXDIR;

	dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, 7 );
	
	while(GS_CSR_FINISH != 1);
	 
	GS_SETREG_CSR_FINISH(1);
	 
	FlushCache(0);
	 
	GS_SETREG_BUSDIR(1);
#endif 
}
