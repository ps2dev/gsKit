//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsCore.c - Primitive creation, handling, and manipulation.
//

#include "gsKit.h"

int gsKit_prim_sprite(int x1, int y1, int x2, int y2, int z, int color)
{
	printf("Drawing Sprite Primitive\n");
	u64* p_store;
	u64* p_data;
	int size = 5;
  
	x1 += 2048 << 4;
	y1 += 2048 << 4;
	x2 += 2048 << 4;
	y2 += 2048 << 4;

	printf("Got Here 1\n");	

	p_store = p_data = dmaKit_spr_alloc( 6*16 );

	*p_data++ = GIF_TAG( size-1, 1, 0, 0, 0, 1 );
	*p_data++ = GIF_AD;

	*p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_SPRITE, 0, 0, 0, g_PbPrimAlphaEnable,
                              0, 0, g_PbPrimContext, 0) ;
	*p_data++ = GS_PRIM;

	printf("Got Here 2\n");	

	*p_data++ = color;
	*p_data++ = GS_RGBAQ;

	*p_data++ = GS_SETREG_XYZ2( x1, y1, z );
	*p_data++ = GS_XYZ2;

	*p_data++ = GS_SETREG_XYZ2( x2, y2, z );
	*p_data++ = GS_XYZ2;

	printf("Got Here 3\n");	

	dmaKit_send_chain_spr( DMA_CHANNEL_GIF, 0, p_store, size );

	printf("Sprite Primitive Drawn\n");
	return 0;
}
