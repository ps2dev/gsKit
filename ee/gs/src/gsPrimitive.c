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

void gsKit_prim_point(GSGLOBAL *gsGlobal, float x1, float y1, float z, u64 color)
{
        u64* p_store;
        u64* p_data;
        int size = 4;
	
	int ix1 = (int)(x1 * 16.0f);
	int iy1 = (int)(y1 * 16.0f);
	int iz = (int)(z * 16.0f);

        ix1 = gsKit_scale(gsGlobal, GS_AXIS_X, ix1);
        iy1 = gsKit_scale(gsGlobal, GS_AXIS_Y, iy1);

        ix1 += gsGlobal->OffsetX << 4;
        iy1 += gsGlobal->OffsetY << 4;

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

        *p_data++ = GS_SETREG_XYZ2( ix1, iy1, iz );
        *p_data++ = GS_XYZ2;

        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}

void gsKit_prim_line(GSGLOBAL *gsGlobal, float x1, float y1, float x2, float y2, float z, u64 color)
{
        u64* p_store;
        u64* p_data;
        int size = 5;

        int ix1 = (int)(x1 * 16.0f);
        int ix2 = (int)(x2 * 16.0f);
        int iy1 = (int)(y1 * 16.0f);
        int iy2 = (int)(y2 * 16.0f);
        int iz = (int)(z * 16.0f);

        ix1 = gsKit_scale(gsGlobal, GS_AXIS_X, ix1);
        ix2 = gsKit_scale(gsGlobal, GS_AXIS_X, ix2);
        iy1 = gsKit_scale(gsGlobal, GS_AXIS_Y, iy1);
        iy2 = gsKit_scale(gsGlobal, GS_AXIS_Y, iy2);

        ix1 += gsGlobal->OffsetX << 4;
        ix2 += gsGlobal->OffsetX << 4;

        iy1 += gsGlobal->OffsetY << 4;
        iy2 += gsGlobal->OffsetY << 4;

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

        *p_data++ = GS_SETREG_XYZ2( ix1, iy1, z );
        *p_data++ = GS_XYZ2;

        *p_data++ = GS_SETREG_XYZ2( ix2, iy2, iz );
        *p_data++ = GS_XYZ2;

        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}


void gsKit_prim_line_strip(GSGLOBAL *gsGlobal, float *LineStrip, int segments, float z, u64 color)
{
        u64* p_store;
        u64* p_data;
        int size = 3 + segments;
	int count;
	int vertexdata[segments*2];

	for(count = 0; count < (segments * 2); count+=2)
	{
	        vertexdata[count] = gsKit_scale(gsGlobal, GS_AXIS_X, (int)(*LineStrip++ * 16.0f));
	        vertexdata[count] += gsGlobal->OffsetX << 4;
	        vertexdata[count+1] = gsKit_scale(gsGlobal, GS_AXIS_Y, (int)(*LineStrip++ * 16.0f));
                vertexdata[count+1] += gsGlobal->OffsetY << 4;
	}
        int iz = (int)(z * 16.0f);

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
	        *p_data++ = GS_SETREG_XYZ2( vertexdata[count], vertexdata[count+1], iz );
	        *p_data++ = GS_XYZ2;
	}

        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}


void gsKit_prim_sprite(GSGLOBAL *gsGlobal, float x1, float y1, float x2, float y2, float z, u64 color)
{
	u64* p_store;
	u64* p_data;
	int size = 5;

        int ix1 = (int)(x1 * 16.0f);
        int ix2 = (int)(x2 * 16.0f);
        int iy1 = (int)(y1 * 16.0f);
        int iy2 = (int)(y2 * 16.0f);
        int iz = (int)(z * 16.0f);

        ix1 = gsKit_scale(gsGlobal, GS_AXIS_X, ix1);
        ix2 = gsKit_scale(gsGlobal, GS_AXIS_X, ix2);
        iy1 = gsKit_scale(gsGlobal, GS_AXIS_Y, iy1);
        iy2 = gsKit_scale(gsGlobal, GS_AXIS_Y, iy2);

	ix1 += gsGlobal->OffsetX << 4;
	ix2 += gsGlobal->OffsetX << 4;

	iy1 += gsGlobal->OffsetY << 4;
	iy2 += gsGlobal->OffsetY << 4;

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

	*p_data++ = GS_SETREG_XYZ2( ix1, iy1, iz );
	*p_data++ = GS_XYZ2;

	*p_data++ = GS_SETREG_XYZ2( ix2, iy2, iz );
	*p_data++ = GS_XYZ2;

        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}

void gsKit_prim_triangle(GSGLOBAL *gsGlobal, float x1, float y1, 
					     float x2, float y2, 
					     float x3, float y3,
					     float z, u64 color)
{
	u64* p_store;
	u64* p_data;
	int size = 6;

        int ix1 = (int)(x1 * 16.0f);
        int ix2 = (int)(x2 * 16.0f);
        int ix3 = (int)(x3 * 16.0f);
        int iy1 = (int)(y1 * 16.0f);
        int iy2 = (int)(y2 * 16.0f);
        int iy3 = (int)(y3 * 16.0f);
        int iz = (int)(z * 16.0f);

	ix1 = gsKit_scale(gsGlobal, GS_AXIS_X, ix1);
	ix2 = gsKit_scale(gsGlobal, GS_AXIS_X, ix2);
	ix3 = gsKit_scale(gsGlobal, GS_AXIS_X, ix3);

	iy1 = gsKit_scale(gsGlobal, GS_AXIS_Y, iy1);
	iy2 = gsKit_scale(gsGlobal, GS_AXIS_Y, iy2);
	iy3 = gsKit_scale(gsGlobal, GS_AXIS_Y, iy3);

	ix1 += gsGlobal->OffsetX << 4;
	ix2 += gsGlobal->OffsetX << 4;
	ix3 += gsGlobal->OffsetX << 4;

	iy1 += gsGlobal->OffsetY << 4;
	iy2 += gsGlobal->OffsetY << 4;
	iy3 += gsGlobal->OffsetY << 4;

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

        *p_data++ = GS_SETREG_XYZ2( ix1, iy1, iz );
        *p_data++ = GS_XYZ2;

        *p_data++ = GS_SETREG_XYZ2( ix2, iy2, iz );
        *p_data++ = GS_XYZ2;

        *p_data++ = GS_SETREG_XYZ2( ix3, iy3, iz );
        *p_data++ = GS_XYZ2;

        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}

void gsKit_prim_triangle_strip(GSGLOBAL *gsGlobal, float *TriStrip, int segments, float z, u64 color)
{
        u64* p_store;
        u64* p_data;
        int size = 3 + segments;
        int count;
        int vertexdata[segments*2];
        
        int iz = (int)(z * 16.0f);

        for(count = 0; count < (segments * 2); count+=2)
        {
                vertexdata[count] = gsKit_scale(gsGlobal, GS_AXIS_X, (int)(*TriStrip++ * 16.0f));
                vertexdata[count] += gsGlobal->OffsetX << 4;
                vertexdata[count+1] = gsKit_scale(gsGlobal, GS_AXIS_Y, (int)(*TriStrip++ * 16.0f));
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
                *p_data++ = GS_SETREG_XYZ2( vertexdata[count], vertexdata[count+1], iz );
                *p_data++ = GS_XYZ2;
        }
        
        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}

void gsKit_prim_triangle_fan(GSGLOBAL *gsGlobal, float *TriFan, int verticies, float z, u64 color)
{
        u64* p_store;
        u64* p_data;
        int size = 3 + verticies;
        int count;
        int vertexdata[verticies*2];

        int iz = (int)(z * 16.0f);

        for(count = 0; count < (verticies * 2); count+=2)
        {
                vertexdata[count] = gsKit_scale(gsGlobal, GS_AXIS_X, (int)(*TriFan++ * 16.0f));
                vertexdata[count] += gsGlobal->OffsetX << 4;
                vertexdata[count+1] = gsKit_scale(gsGlobal, GS_AXIS_Y, (int)(*TriFan++ * 16.0f));
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

        *p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_TRIFAN, 0, 0, gsGlobal->PrimFogEnable,
                                    gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
                                    0, gsGlobal->PrimContext, 0) ;

        *p_data++ = GS_PRIM;

        *p_data++ = color;
        *p_data++ = GS_RGBAQ;

        for(count = 0; count < (verticies * 2); count+=2)
        {
                *p_data++ = GS_SETREG_XYZ2( vertexdata[count], vertexdata[count+1], iz );
                *p_data++ = GS_XYZ2;
        }

        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}


void gsKit_prim_triangle_gouraud(GSGLOBAL *gsGlobal, float x1, float y1,
                                                     float x2, float y2,
                                                     float x3, float y3, float z, 
                                                     u64 color1, u64 color2, u64 color3)
{
	u64* p_store;
	u64* p_data;
	int size = 8;

        int ix1 = (int)(x1 * 16.0f);
        int ix2 = (int)(x2 * 16.0f);
        int ix3 = (int)(x3 * 16.0f);
        int iy1 = (int)(y1 * 16.0f);
        int iy2 = (int)(y2 * 16.0f);
        int iy3 = (int)(y3 * 16.0f);
        int iz = (int)(z * 16.0f);

	ix1 = gsKit_scale(gsGlobal, GS_AXIS_X, ix1);
	ix2 = gsKit_scale(gsGlobal, GS_AXIS_X, ix2);
	ix3 = gsKit_scale(gsGlobal, GS_AXIS_X, ix3);

	iy1 = gsKit_scale(gsGlobal, GS_AXIS_Y, iy1);
	iy2 = gsKit_scale(gsGlobal, GS_AXIS_Y, iy2);
	iy3 = gsKit_scale(gsGlobal, GS_AXIS_Y, iy3);

	ix1 += gsGlobal->OffsetX << 4;
	ix2 += gsGlobal->OffsetX << 4;
	ix3 += gsGlobal->OffsetX << 4;

	iy1 += gsGlobal->OffsetY << 4;
	iy2 += gsGlobal->OffsetY << 4;
	iy3 += gsGlobal->OffsetY << 4;

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

        *p_data++ = GS_SETREG_XYZ2( ix1, iy1, iz );   
        *p_data++ = GS_XYZ2;
  
        *p_data++ = color2;
        *p_data++ = GS_RGBAQ;

        *p_data++ = GS_SETREG_XYZ2( ix2, iy2, iz );
        *p_data++ = GS_XYZ2;

        *p_data++ = color3;
        *p_data++ = GS_RGBAQ;

        *p_data++ = GS_SETREG_XYZ2( ix3, iy3, iz );
        *p_data++ = GS_XYZ2;
        
        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}

void gsKit_prim_quad(GSGLOBAL *gsGlobal, float x1, float y1,
                                         float x2, float y2,
                                         float x3, float y3,
                                         float x4, float y4, float z, u64 color)
{
	u64* p_store;
	u64* p_data;
	int size = 7;
        
        int ix1 = (int)(x1 * 16.0f);
        int ix2 = (int)(x2 * 16.0f);
        int ix3 = (int)(x3 * 16.0f);
        int ix4 = (int)(x4 * 16.0f);
        int iy1 = (int)(y1 * 16.0f);
        int iy2 = (int)(y2 * 16.0f);
        int iy3 = (int)(y3 * 16.0f);
        int iy4 = (int)(y4 * 16.0f);
        int iz = (int)(z * 16.0f);

	ix1 = gsKit_scale(gsGlobal, GS_AXIS_X, ix1);
	ix2 = gsKit_scale(gsGlobal, GS_AXIS_X, ix2);
	ix3 = gsKit_scale(gsGlobal, GS_AXIS_X, ix3);
	ix4 = gsKit_scale(gsGlobal, GS_AXIS_X, ix4);
                
	iy1 = gsKit_scale(gsGlobal, GS_AXIS_Y, iy1);
	iy2 = gsKit_scale(gsGlobal, GS_AXIS_Y, iy2);
	iy3 = gsKit_scale(gsGlobal, GS_AXIS_Y, iy3);
	iy4 = gsKit_scale(gsGlobal, GS_AXIS_Y, iy4);
        
	ix1 += gsGlobal->OffsetX << 4;
	ix2 += gsGlobal->OffsetX << 4;
	ix3 += gsGlobal->OffsetX << 4;
	ix4 += gsGlobal->OffsetX << 4;

	iy1 += gsGlobal->OffsetY << 4;
	iy2 += gsGlobal->OffsetY << 4;
	iy3 += gsGlobal->OffsetY << 4;
	iy4 += gsGlobal->OffsetY << 4;

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

        *p_data++ = GS_SETREG_XYZ2( ix1, iy1, iz );
        *p_data++ = GS_XYZ2;

        *p_data++ = GS_SETREG_XYZ2( ix2, iy2, iz );
        *p_data++ = GS_XYZ2;

        *p_data++ = GS_SETREG_XYZ2( ix3, iy3, iz );
        *p_data++ = GS_XYZ2;

        *p_data++ = GS_SETREG_XYZ2( ix4, iy4, iz );
        *p_data++ = GS_XYZ2;

        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}


void gsKit_prim_quad_gouraud(GSGLOBAL *gsGlobal, float x1, float y1, 
						 float x2, float y2, 
						 float x3, float y3,
						 float x4, float y4, float z, 
						 u64 color1, u64 color2,
						 u64 color3, u64 color4)
{
	u64* p_store;
	u64* p_data;
	int size = 10;

        int ix1 = (int)(x1 * 16.0f);
        int ix2 = (int)(x2 * 16.0f);
        int ix3 = (int)(x3 * 16.0f);
        int ix4 = (int)(x4 * 16.0f);
        int iy1 = (int)(y1 * 16.0f);
        int iy2 = (int)(y2 * 16.0f);
        int iy3 = (int)(y3 * 16.0f);
        int iy4 = (int)(y4 * 16.0f);
        int iz = (int)(z * 16.0f);

	ix1 = gsKit_scale(gsGlobal, GS_AXIS_X, ix1);
	ix2 = gsKit_scale(gsGlobal, GS_AXIS_X, ix2);
	ix3 = gsKit_scale(gsGlobal, GS_AXIS_X, ix3);
	ix4 = gsKit_scale(gsGlobal, GS_AXIS_X, ix4);

	iy1 = gsKit_scale(gsGlobal, GS_AXIS_Y, iy1);
	iy2 = gsKit_scale(gsGlobal, GS_AXIS_Y, iy2);
	iy3 = gsKit_scale(gsGlobal, GS_AXIS_Y, iy3);
	iy4 = gsKit_scale(gsGlobal, GS_AXIS_Y, iy4);

	ix1 += gsGlobal->OffsetX << 4;
	ix2 += gsGlobal->OffsetX << 4;
	ix3 += gsGlobal->OffsetX << 4;
	ix4 += gsGlobal->OffsetX << 4;
        
	iy1 += gsGlobal->OffsetY << 4;
	iy2 += gsGlobal->OffsetY << 4;
	iy3 += gsGlobal->OffsetY << 4;
	iy4 += gsGlobal->OffsetY << 4;

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

	*p_data++ = GS_SETREG_XYZ2( ix1, iy1, iz );
	*p_data++ = GS_XYZ2;
	
	*p_data++ = color2;
	*p_data++ = GS_RGBAQ;
	
	*p_data++ = GS_SETREG_XYZ2( ix2, iy2, iz );
	*p_data++ = GS_XYZ2;

	*p_data++ = color3;
	*p_data++ = GS_RGBAQ;

	*p_data++ = GS_SETREG_XYZ2( ix3, iy3, iz );
	*p_data++ = GS_XYZ2;

	*p_data++ = color4;
	*p_data++ = GS_RGBAQ;

	*p_data++ = GS_SETREG_XYZ2( ix4, iy4, iz );
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

