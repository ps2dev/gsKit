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

int gsKit_prim_sprite_ztest(GSGLOBAL gsGlobal, float x1, float y1, float x2, float y2, int z, int color)
{
	printf("Drawing Sprite Primitive With Z Test\n");
	u64* p_store;
	u64* p_data;
	int size = 5;
  
	x1 += gsGlobal.OffsetX << 4;
	x2 += gsGlobal.OffsetX << 4;

	y1 += gsGlobal.OffsetY << 4;
	y2 += gsGlobal.OffsetY << 4;


//	x1 = gsKit_scale(gsGlobal, GS_AXIS_X, x1);
//	x2 = gsKit_scale(gsGlobal, GS_AXIS_X, x2);
//	y1 = gsKit_scale(gsGlobal, GS_AXIS_Y, y1);
//	y2 = gsKit_scale(gsGlobal, GS_AXIS_Y, y2);

	p_store = p_data = dmaKit_spr_alloc( 6*16 );

	if( gsGlobal.PrimAlphaEnable == 1 )
		size++;   

	*p_data++ = GIF_TAG( size-1, 1, 0, 0, 0, 1 );
	*p_data++ = GIF_AD;


	if( gsGlobal.PrimAlphaEnable == 1 )
	{
		*p_data++ = gsGlobal.PrimAlpha;
		*p_data++ = GS_ALPHA_1+gsGlobal.PrimContext;
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

int gsKit_prim_sprite(GSGLOBAL gsGlobal, float x1, float y1, float x2, float y2, int z, int color)
{
	printf("Drawing Sprite Primitive Without Z Test\n");
	u64* p_store;
	u64* p_data;
	int size = 7;

	x1 += gsGlobal.OffsetX << 4;
	x2 += gsGlobal.OffsetX << 4;

	y1 += gsGlobal.OffsetY << 4;
	y2 += gsGlobal.OffsetY << 4;

//	x1 += gsKit_scale(gsGlobal, GS_AXIS_X, x1);
//	y1 += gsKit_scale(gsGlobal, GS_AXIS_Y, y1);
//	x2 += gsKit_scale(gsGlobal, GS_AXIS_X, x2);
//	y2 += gsKit_scale(gsGlobal, GS_AXIS_Y, y2);




        p_store = p_data = dmaKit_spr_alloc( 8*16 );

        if( gsGlobal.PrimAlphaEnable == 1 )
                size++;

        *p_data++ = GIF_TAG( size-1, 1, 0, 0, 0, 1 );
        *p_data++ = GIF_AD;


        if( gsGlobal.PrimAlphaEnable == 1 )
        {
                *p_data++ = gsGlobal.PrimAlpha;
                *p_data++ = GS_ALPHA_1+gsGlobal.PrimContext;
        }

        *p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_SPRITE, 0, 0, 0, gsGlobal.PrimAlphaEnable,
                              0, 0, gsGlobal.PrimContext, 0) ;

	*p_data++ = GS_PRIM;

	*p_data++ = GS_SETREG_TEST( 1, 7, 0xFF, 0, 0, 0, 1, 1 );
	*p_data++ = GS_TEST_1+gsGlobal.PrimContext;

	*p_data++ = color;
	*p_data++ = GS_RGBAQ;

	*p_data++ = GS_SETREG_XYZ2( x1, y1, z );
	*p_data++ = GS_XYZ2;

	*p_data++ = GS_SETREG_XYZ2( x2, y2, z );
	*p_data++ = GS_XYZ2;

	*p_data++ = GS_SETREG_TEST( 1, 7, 0xFF, 0, 0, 0, 1, 2 );
	*p_data++ = GS_TEST_1+gsGlobal.PrimContext;

        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );

        printf("Sprite Primitive Drawn\n");
        return 0;
}

