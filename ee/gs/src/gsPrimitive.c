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

void gsKit_prim_point(GSGLOBAL *gsGlobal, int x1, int y1, int z, u64 color)
{
        u64* p_store;
        u64* p_data;
        int size = 4;

        x1 = gsKit_scale(gsGlobal, GS_AXIS_X, x1);
        y1 = gsKit_scale(gsGlobal, GS_AXIS_Y, y1);

        x1 += gsGlobal->OffsetX << 4;
        y1 += gsGlobal->OffsetY << 4;

        if( gsGlobal->PrimAlphaEnable == 1 )
                size++;

        p_store = p_data = dmaKit_spr_alloc( size*16 );

        *p_data++ = GIF_TAG( size - 1, 1, 0, 0, 0, 1 );
        *p_data++ = GIF_AD;

        if( gsGlobal->PrimAlphaEnable == 1 )
        {
                *p_data++ = gsGlobal->PrimAlpha;
                *p_data++ = GS_ALPHA_1+gsGlobal->PrimContext;
        }

        *p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_POINT, 0, 0, gsGlobal->PrimFogEnable,
                                    gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
                                    0, gsGlobal->PrimContext, 0) ;

        *p_data++ = GS_PRIM;

        *p_data++ = color;
        *p_data++ = GS_RGBAQ;

        *p_data++ = GS_SETREG_XYZ2( x1, y1, z );
        *p_data++ = GS_XYZ2;

        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}

void gsKit_prim_line(GSGLOBAL *gsGlobal, int x1, int y1, int x2, int y2, int z, u64 color)
{
        u64* p_store;
        u64* p_data;
        int size = 5;

        x1 = gsKit_scale(gsGlobal, GS_AXIS_X, x1);
        x2 = gsKit_scale(gsGlobal, GS_AXIS_X, x2);
        y1 = gsKit_scale(gsGlobal, GS_AXIS_Y, y1);
        y2 = gsKit_scale(gsGlobal, GS_AXIS_Y, y2);

        x1 += gsGlobal->OffsetX << 4;
        x2 += gsGlobal->OffsetX << 4;

        y1 += gsGlobal->OffsetY << 4;
        y2 += gsGlobal->OffsetY << 4;

        if( gsGlobal->PrimAlphaEnable == 1 )
                size++;

        p_store = p_data = dmaKit_spr_alloc( size*16 );

        *p_data++ = GIF_TAG( size - 1, 1, 0, 0, 0, 1 );
        *p_data++ = GIF_AD;

        if( gsGlobal->PrimAlphaEnable == 1 )
        {
                *p_data++ = gsGlobal->PrimAlpha;
                *p_data++ = GS_ALPHA_1+gsGlobal->PrimContext;
        }

        *p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_LINE, 0, 0, gsGlobal->PrimFogEnable,
                                    gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
                                    0, gsGlobal->PrimContext, 0) ;

        *p_data++ = GS_PRIM;

        *p_data++ = color;
        *p_data++ = GS_RGBAQ;

        *p_data++ = GS_SETREG_XYZ2( x1, y1, z );
        *p_data++ = GS_XYZ2;

        *p_data++ = GS_SETREG_XYZ2( x2, y2, z );
        *p_data++ = GS_XYZ2;

        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}


void gsKit_prim_line_strip(GSGLOBAL *gsGlobal, int *LineStrip, int segments, int z, u64 color)
{
        u64* p_store;
        u64* p_data;
        int size = 3 + segments;
	int count;
	int vertexdata[segments*2];

	for(count = 0; count < (segments * 2); count+=2)
	{
	        vertexdata[count] = gsKit_scale(gsGlobal, GS_AXIS_X, *LineStrip++);
	        vertexdata[count] += gsGlobal->OffsetX << 4;
	        vertexdata[count+1] = gsKit_scale(gsGlobal, GS_AXIS_Y, *LineStrip++);
                vertexdata[count+1] += gsGlobal->OffsetY << 4;
	}
		
        if( gsGlobal->PrimAlphaEnable == 1 )
                size++;
        
        p_store = p_data = dmaKit_spr_alloc( size*16 );

        *p_data++ = GIF_TAG( size - 1, 1, 0, 0, 0, 1 );
        *p_data++ = GIF_AD;
        
        if( gsGlobal->PrimAlphaEnable == 1 )
        {
                *p_data++ = gsGlobal->PrimAlpha;
                *p_data++ = GS_ALPHA_1+gsGlobal->PrimContext;
        }
        
        *p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_LINESTRIP, 0, 0, gsGlobal->PrimFogEnable,
                                    gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
                                    0, gsGlobal->PrimContext, 0) ;
        
        *p_data++ = GS_PRIM;

        *p_data++ = color;
        *p_data++ = GS_RGBAQ;

        for(count = 0; count < (segments * 2); count+=2)
        {
	        *p_data++ = GS_SETREG_XYZ2( vertexdata[count], vertexdata[count+1], z );
	        *p_data++ = GS_XYZ2;
	}

        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}


void gsKit_prim_sprite(GSGLOBAL *gsGlobal, int x1, int y1, int x2, int y2, int z, u64 color)
{
	u64* p_store;
	u64* p_data;
	int size = 5;

        x1 = gsKit_scale(gsGlobal, GS_AXIS_X, x1);
        x2 = gsKit_scale(gsGlobal, GS_AXIS_X, x2);
        y1 = gsKit_scale(gsGlobal, GS_AXIS_Y, y1);
        y2 = gsKit_scale(gsGlobal, GS_AXIS_Y, y2);

	x1 += gsGlobal->OffsetX << 4;
	x2 += gsGlobal->OffsetX << 4;

	y1 += gsGlobal->OffsetY << 4;
	y2 += gsGlobal->OffsetY << 4;

        if( gsGlobal->PrimAlphaEnable == 1 )
                size++;

        p_store = p_data = dmaKit_spr_alloc( size*16 );

        *p_data++ = GIF_TAG( size - 1, 1, 0, 0, 0, 1 );
        *p_data++ = GIF_AD;

        if( gsGlobal->PrimAlphaEnable == 1 )
        {
                *p_data++ = gsGlobal->PrimAlpha;
                *p_data++ = GS_ALPHA_1+gsGlobal->PrimContext;
        }

	*p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_SPRITE, 0, 0, gsGlobal->PrimFogEnable,
				    gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
				    0, gsGlobal->PrimContext, 0) ;

	*p_data++ = GS_PRIM;

	*p_data++ = color;
	*p_data++ = GS_RGBAQ;

	*p_data++ = GS_SETREG_XYZ2( x1, y1, z );
	*p_data++ = GS_XYZ2;

	*p_data++ = GS_SETREG_XYZ2( x2, y2, z );
	*p_data++ = GS_XYZ2;

        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}

void gsKit_prim_triangle(GSGLOBAL *gsGlobal, int x1, int y1, 
					     int x2, int y2, 
					     int x3, int y3,
					     int z, u64 color)
{
	u64* p_store;
	u64* p_data;
	int size = 6;

	x1 = gsKit_scale(gsGlobal, GS_AXIS_X, x1);
	x2 = gsKit_scale(gsGlobal, GS_AXIS_X, x2);
	x3 = gsKit_scale(gsGlobal, GS_AXIS_X, x3);

	y1 = gsKit_scale(gsGlobal, GS_AXIS_Y, y1);
	y2 = gsKit_scale(gsGlobal, GS_AXIS_Y, y2);
	y3 = gsKit_scale(gsGlobal, GS_AXIS_Y, y3);

	x1 += gsGlobal->OffsetX << 4;
	x2 += gsGlobal->OffsetX << 4;
	x3 += gsGlobal->OffsetX << 4;

	y1 += gsGlobal->OffsetY << 4;
	y2 += gsGlobal->OffsetY << 4;
	y3 += gsGlobal->OffsetY << 4;

        if( gsGlobal->PrimAlphaEnable == 1 )
                size++;

        p_store = p_data = dmaKit_spr_alloc( size*16 );

        *p_data++ = GIF_TAG( size - 1, 1, 0, 0, 0, 1 );
        *p_data++ = GIF_AD;

        if( gsGlobal->PrimAlphaEnable == 1 )
        {
                *p_data++ = gsGlobal->PrimAlpha;
                *p_data++ = GS_ALPHA_1+gsGlobal->PrimContext;
        }
        
	*p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_TRIANGLE, 0, 0, gsGlobal->PrimFogEnable,
				    gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
				    0, gsGlobal->PrimContext, 0) ;


        *p_data++ = GS_PRIM;
        
        *p_data++ = color;
        *p_data++ = GS_RGBAQ;

        *p_data++ = GS_SETREG_XYZ2( x1, y1, z );
        *p_data++ = GS_XYZ2;

        *p_data++ = GS_SETREG_XYZ2( x2, y2, z );
        *p_data++ = GS_XYZ2;

        *p_data++ = GS_SETREG_XYZ2( x3, y3, z );
        *p_data++ = GS_XYZ2;

        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}

void gsKit_prim_triangle_strip(GSGLOBAL *gsGlobal, int *TriStrip, int segments, int z, u64 color)
{
        u64* p_store;
        u64* p_data;
        int size = 3 + segments;
        int count;
        int vertexdata[segments*2];
        
        for(count = 0; count < (segments * 2); count+=2)
        {
                vertexdata[count] = gsKit_scale(gsGlobal, GS_AXIS_X, *TriStrip++);
                vertexdata[count] += gsGlobal->OffsetX << 4;
                vertexdata[count+1] = gsKit_scale(gsGlobal, GS_AXIS_Y, *TriStrip++);
                vertexdata[count+1] += gsGlobal->OffsetY << 4;
        }

        if( gsGlobal->PrimAlphaEnable == 1 )
                size++;

        p_store = p_data = dmaKit_spr_alloc( size*16 );

        *p_data++ = GIF_TAG( size - 1, 1, 0, 0, 0, 1 );
        *p_data++ = GIF_AD;

        if( gsGlobal->PrimAlphaEnable == 1 )
        {
                *p_data++ = gsGlobal->PrimAlpha;
                *p_data++ = GS_ALPHA_1+gsGlobal->PrimContext;
        }

        *p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_TRISTRIP, 0, 0, gsGlobal->PrimFogEnable,
                                    gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
                                    0, gsGlobal->PrimContext, 0) ;

        *p_data++ = GS_PRIM;

        *p_data++ = color;
        *p_data++ = GS_RGBAQ;

        for(count = 0; count < (segments * 2); count+=2)
        {
                *p_data++ = GS_SETREG_XYZ2( vertexdata[count], vertexdata[count+1], z );
                *p_data++ = GS_XYZ2;
        }
        
        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}

void gsKit_prim_triangle_gouraud(GSGLOBAL *gsGlobal, int x1, int y1,
                                                     int x2, int y2,
                                                     int x3, int y3, int z, 
                                                     u64 color1, u64 color2, u64 color3)
{
	u64* p_store;
	u64* p_data;
	int size = 8;

	x1 = gsKit_scale(gsGlobal, GS_AXIS_X, x1);
	x2 = gsKit_scale(gsGlobal, GS_AXIS_X, x2);
	x3 = gsKit_scale(gsGlobal, GS_AXIS_X, x3);

	y1 = gsKit_scale(gsGlobal, GS_AXIS_Y, y1);
	y2 = gsKit_scale(gsGlobal, GS_AXIS_Y, y2);
	y3 = gsKit_scale(gsGlobal, GS_AXIS_Y, y3);

	x1 += gsGlobal->OffsetX << 4;
	x2 += gsGlobal->OffsetX << 4;
	x3 += gsGlobal->OffsetX << 4;

	y1 += gsGlobal->OffsetY << 4;
	y2 += gsGlobal->OffsetY << 4;
	y3 += gsGlobal->OffsetY << 4;

	if( gsGlobal->PrimAlphaEnable == 1 )
		size++;

	p_store = p_data = dmaKit_spr_alloc( size*16 );

	*p_data++ = GIF_TAG( size - 1, 1, 0, 0, 0, 1 );
	*p_data++ = GIF_AD;

	if( gsGlobal->PrimAlphaEnable == 1 )
	{
		*p_data++ = gsGlobal->PrimAlpha;
		*p_data++ = GS_ALPHA_1+gsGlobal->PrimContext;
	}

	*p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_TRIANGLE, 1, 0, gsGlobal->PrimFogEnable,
				    gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
				    0, gsGlobal->PrimContext, 0) ;

        *p_data++ = GS_PRIM;

        *p_data++ = color1;
        *p_data++ = GS_RGBAQ;

        *p_data++ = GS_SETREG_XYZ2( x1, y1, z );   
        *p_data++ = GS_XYZ2;
  
        *p_data++ = color2;
        *p_data++ = GS_RGBAQ;

        *p_data++ = GS_SETREG_XYZ2( x2, y2, z );
        *p_data++ = GS_XYZ2;

        *p_data++ = color3;
        *p_data++ = GS_RGBAQ;

        *p_data++ = GS_SETREG_XYZ2( x3, y3, z );
        *p_data++ = GS_XYZ2;
        
        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}

void gsKit_prim_quad(GSGLOBAL *gsGlobal, int x1, int y1,
                                         int x2, int y2,
                                         int x3, int y3,
                                         int x4, int y4, int z, u64 color)
{
	u64* p_store;
	u64* p_data;
	int size = 7;
        
	x1 = gsKit_scale(gsGlobal, GS_AXIS_X, x1);
	x2 = gsKit_scale(gsGlobal, GS_AXIS_X, x2);
	x3 = gsKit_scale(gsGlobal, GS_AXIS_X, x3);
	x4 = gsKit_scale(gsGlobal, GS_AXIS_X, x4);
                
	y1 = gsKit_scale(gsGlobal, GS_AXIS_Y, y1);
	y2 = gsKit_scale(gsGlobal, GS_AXIS_Y, y2);
	y3 = gsKit_scale(gsGlobal, GS_AXIS_Y, y3);
	y4 = gsKit_scale(gsGlobal, GS_AXIS_Y, y4);
        
	x1 += gsGlobal->OffsetX << 4;
	x2 += gsGlobal->OffsetX << 4;
	x3 += gsGlobal->OffsetX << 4;
	x4 += gsGlobal->OffsetX << 4;

	y1 += gsGlobal->OffsetY << 4;
	y2 += gsGlobal->OffsetY << 4;
	y3 += gsGlobal->OffsetY << 4;
	y4 += gsGlobal->OffsetY << 4;

        if( gsGlobal->PrimAlphaEnable == 1 )
        size++;

        p_store = p_data = dmaKit_spr_alloc( size*16 );

        *p_data++ = GIF_TAG( size - 1, 1, 0, 0, 0, 1 );
        *p_data++ = GIF_AD;

        if( gsGlobal->PrimAlphaEnable == 1 )
        {
        *p_data++ = gsGlobal->PrimAlpha;
        *p_data++ = GS_ALPHA_1+gsGlobal->PrimContext;
        }

        *p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_TRISTRIP, 0, 0, gsGlobal->PrimFogEnable,
                                    gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
                                    0, gsGlobal->PrimContext, 0) ;

        *p_data++ = GS_PRIM;

        *p_data++ = color;
        *p_data++ = GS_RGBAQ;

        *p_data++ = GS_SETREG_XYZ2( x1, y1, z );
        *p_data++ = GS_XYZ2;

        *p_data++ = GS_SETREG_XYZ2( x2, y2, z );
        *p_data++ = GS_XYZ2;

        *p_data++ = GS_SETREG_XYZ2( x3, y3, z );
        *p_data++ = GS_XYZ2;

        *p_data++ = GS_SETREG_XYZ2( x4, y4, z );
        *p_data++ = GS_XYZ2;

        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}


void gsKit_prim_quad_gouraud(GSGLOBAL *gsGlobal, int x1, int y1, 
						 int x2, int y2, 
						 int x3, int y3,
						 int x4, int y4, int z, 
						 u64 color1, u64 color2,
						 u64 color3, u64 color4)
{
	u64* p_store;
	u64* p_data;
	int size = 10;

	x1 = gsKit_scale(gsGlobal, GS_AXIS_X, x1);
	x2 = gsKit_scale(gsGlobal, GS_AXIS_X, x2);
	x3 = gsKit_scale(gsGlobal, GS_AXIS_X, x3);
	x4 = gsKit_scale(gsGlobal, GS_AXIS_X, x4);

	y1 = gsKit_scale(gsGlobal, GS_AXIS_Y, y1);
	y2 = gsKit_scale(gsGlobal, GS_AXIS_Y, y2);
	y3 = gsKit_scale(gsGlobal, GS_AXIS_Y, y3);
	y4 = gsKit_scale(gsGlobal, GS_AXIS_Y, y4);

	x1 += gsGlobal->OffsetX << 4;
	x2 += gsGlobal->OffsetX << 4;
	x3 += gsGlobal->OffsetX << 4;
	x4 += gsGlobal->OffsetX << 4;
        
	y1 += gsGlobal->OffsetY << 4;
	y2 += gsGlobal->OffsetY << 4;
	y3 += gsGlobal->OffsetY << 4;
	y4 += gsGlobal->OffsetY << 4;

	if( gsGlobal->PrimAlphaEnable == 1 )
	size++;

	p_store = p_data = dmaKit_spr_alloc( size*16 );
         
	*p_data++ = GIF_TAG( size - 1, 1, 0, 0, 0, 1 );
	*p_data++ = GIF_AD;

	if( gsGlobal->PrimAlphaEnable == 1 )
	{
	*p_data++ = gsGlobal->PrimAlpha;
	*p_data++ = GS_ALPHA_1+gsGlobal->PrimContext;
	}
        
	*p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_TRISTRIP, 1, 0, gsGlobal->PrimFogEnable, 
				    gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
				    0, gsGlobal->PrimContext, 0) ;
        
	*p_data++ = GS_PRIM;

	*p_data++ = color1;
	*p_data++ = GS_RGBAQ;

	*p_data++ = GS_SETREG_XYZ2( x1, y1, z );
	*p_data++ = GS_XYZ2;
	
	*p_data++ = color2;
	*p_data++ = GS_RGBAQ;
	
	*p_data++ = GS_SETREG_XYZ2( x2, y2, z );
	*p_data++ = GS_XYZ2;

	*p_data++ = color3;
	*p_data++ = GS_RGBAQ;

	*p_data++ = GS_SETREG_XYZ2( x3, y3, z );
	*p_data++ = GS_XYZ2;

	*p_data++ = color4;
	*p_data++ = GS_RGBAQ;

	*p_data++ = GS_SETREG_XYZ2( x4, y4, z );
	*p_data++ = GS_XYZ2;

	dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}

void gsKit_zblank(GSGLOBAL *gsGlobal)
{
        u64* p_store;
        u64* p_data;

        int x1 = gsKit_scale(gsGlobal, GS_AXIS_X, 0);
        int x2 = gsKit_scale(gsGlobal, GS_AXIS_X, gsGlobal->Width);
        int y1 = gsKit_scale(gsGlobal, GS_AXIS_Y, 0);
        int y2 = gsKit_scale(gsGlobal, GS_AXIS_Y, gsGlobal->Height);

        x1 += gsGlobal->OffsetX << 4;
        x2 += gsGlobal->OffsetX << 4;

        y1 += gsGlobal->OffsetY << 4;
        y2 += gsGlobal->OffsetY << 4;

        p_store = p_data = dmaKit_spr_alloc( 6*16 );

        *p_data++ = GIF_TAG( 5, 1, 0, 0, 0, 1 );
        *p_data++ = GIF_AD;

        *p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_SPRITE, 0, 0, 0, 0,
                                    0, 0, gsGlobal->PrimContext, 0) ;

        *p_data++ = GS_PRIM;

	*p_data++ = 0x00030000;
	*p_data++ = GS_TEST_1;
	
        *p_data++ = 0;
        *p_data++ = GS_RGBAQ;

        *p_data++ = GS_SETREG_XYZ2( x1, y1, 0 );
        *p_data++ = GS_XYZ2;

        *p_data++ = GS_SETREG_XYZ2( x2, y2, 0 );
        *p_data++ = GS_XYZ2;

        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, 6 );
}

