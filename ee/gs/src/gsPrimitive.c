//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsPrimitive.c - Primitive creation, handling, and manipulation.
//
// Parts taken from emoon's BreakPoint Demo Library
//

#include "gsKit.h"

int gsKit_prim_sprite(GSGLOBAL gsGlobal, int x1, int y1, int x2, int y2, int z, u64 color)
{
	u64* p_store;
	u64* p_data;
	int size = 5;
	printf("Drawing Sprite Primitive\n");

//        x1 = gsKit_scale(gsGlobal, GS_AXIS_X, x1);
//        x2 = gsKit_scale(gsGlobal, GS_AXIS_X, x2);
//        y1 = gsKit_scale(gsGlobal, GS_AXIS_Y, y1);
//        y2 = gsKit_scale(gsGlobal, GS_AXIS_Y, y2);

	x1 += gsGlobal.OffsetX << 4;
	x2 += gsGlobal.OffsetX << 4;

	y1 += gsGlobal.OffsetY << 4;
	y2 += gsGlobal.OffsetY << 4;

        if( gsGlobal.PrimAlphaEnable == 1 )
                size++;

        p_store = p_data = dmaKit_spr_alloc( size*16 );

        *p_data++ = GIF_TAG( size - 1, 1, 0, 0, 0, 1 );
        *p_data++ = GIF_AD;

        if( gsGlobal.PrimAlphaEnable == 1 )
        {
                *p_data++ = gsGlobal.PrimAlpha;
                *p_data++ = GS_ALPHA_1+gsGlobal.PrimContext;
		size++;
        }

        *p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_SPRITE, 0, 0, 0, gsGlobal.PrimAlphaEnable,
                                    0, 0, gsGlobal.PrimContext, 0) ;

	*p_data++ = GS_PRIM;

	*p_data++ = color;
	*p_data++ = GS_RGBAQ;

	*p_data++ = GS_SETREG_XYZ2( x1, y1, z );
	*p_data++ = GS_XYZ2;

	*p_data++ = GS_SETREG_XYZ2( x2, y2, z );
	*p_data++ = GS_XYZ2;

        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );

        printf("Sprite Primitive Drawn\n");
        return 0;
}

