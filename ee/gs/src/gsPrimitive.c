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

void gsKit_prim_point(GSGLOBAL *gsGlobal, float x, float y, float z, u64 color)
{
        u64* p_store;
        u64* p_data;
        int size = 4;
	
        int ix = gsKit_scale(gsGlobal, GS_AXIS_X, x);
        int iy = gsKit_scale(gsGlobal, GS_AXIS_Y, y);
        int iz = gsKit_scale(gsGlobal, GS_AXIS_Z, z);

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

        *p_data++ = GS_SETREG_XYZ2( ix, iy, iz );
        *p_data++ = GS_XYZ2;

        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}

void gsKit_prim_line_3d(GSGLOBAL *gsGlobal, float x1, float y1, float z1, float x2, float y2, float z2, u64 color)
{
        u64* p_store;
        u64* p_data;
        int size = 5;

        int ix1 = gsKit_scale(gsGlobal, GS_AXIS_X, x1);
        int ix2 = gsKit_scale(gsGlobal, GS_AXIS_X, x2);
        int iy1 = gsKit_scale(gsGlobal, GS_AXIS_Y, y1);
        int iy2 = gsKit_scale(gsGlobal, GS_AXIS_Y, y2);
        
	int iz1 = gsKit_scale(gsGlobal, GS_AXIS_Z, z1);
	int iz2 = gsKit_scale(gsGlobal, GS_AXIS_Z, z2);

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

        *p_data++ = GS_SETREG_XYZ2( ix1, iy1, iz1 );
        *p_data++ = GS_XYZ2;

        *p_data++ = GS_SETREG_XYZ2( ix2, iy2, iz2 );
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
	        vertexdata[count] = gsKit_scale(gsGlobal, GS_AXIS_X, *LineStrip++);
	        vertexdata[count+1] = gsKit_scale(gsGlobal, GS_AXIS_Y, *LineStrip++);
	}
        int iz = gsKit_scale(gsGlobal, GS_AXIS_Z, z);

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

void gsKit_prim_line_strip_3d(GSGLOBAL *gsGlobal, float *LineStrip, int segments, u64 color)
{
        u64* p_store;
        u64* p_data;
        int size = 3 + segments;
	int count;
	int vertexdata[segments*3];

	for(count = 0; count < (segments * 3); count+=3)
	{
	        vertexdata[count] = gsKit_scale(gsGlobal, GS_AXIS_X, *LineStrip++);
	        vertexdata[count+1] = gsKit_scale(gsGlobal, GS_AXIS_Y, *LineStrip++);
		vertexdata[count+2] = gsKit_scale(gsGlobal, GS_AXIS_Z, *LineStrip++);
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

        for(count = 0; count < (segments * 3); count+=3)
        {
	        *p_data++ = GS_SETREG_XYZ2( vertexdata[count], vertexdata[count+1], vertexdata[count+2] );
	        *p_data++ = GS_XYZ2;
	}

        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}

void gsKit_prim_sprite(GSGLOBAL *gsGlobal, float x1, float y1, float x2, float y2, float z, u64 color)
{
	u64* p_store;
	u64* p_data;
	int size = 5;

        int ix1 = gsKit_scale(gsGlobal, GS_AXIS_X, x1);
        int ix2 = gsKit_scale(gsGlobal, GS_AXIS_X, x2);
        int iy1 = gsKit_scale(gsGlobal, GS_AXIS_Y, y1);
        int iy2 = gsKit_scale(gsGlobal, GS_AXIS_Y, y2);
        int iz = gsKit_scale(gsGlobal, GS_AXIS_Z, z);

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

void gsKit_prim_triangle_3d(GSGLOBAL *gsGlobal, float x1, float y1, float z1, 
			     float x2, float y2, float z2,
			     float x3, float y3, float z3, u64 color)
{
	u64* p_store;
	u64* p_data;
	int size = 6;

	int ix1 = gsKit_scale(gsGlobal, GS_AXIS_X, x1);
	int ix2 = gsKit_scale(gsGlobal, GS_AXIS_X, x2);
	int ix3 = gsKit_scale(gsGlobal, GS_AXIS_X, x3);

	int iy1 = gsKit_scale(gsGlobal, GS_AXIS_Y, y1);
	int iy2 = gsKit_scale(gsGlobal, GS_AXIS_Y, y2);
	int iy3 = gsKit_scale(gsGlobal, GS_AXIS_Y, y3);

	int iz1 = gsKit_scale(gsGlobal, GS_AXIS_Z, z1);
	int iz2 = gsKit_scale(gsGlobal, GS_AXIS_Z, z2);
	int iz3 = gsKit_scale(gsGlobal, GS_AXIS_Z, z3);

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

        *p_data++ = GS_SETREG_XYZ2( ix1, iy1, iz1 );
        *p_data++ = GS_XYZ2;

        *p_data++ = GS_SETREG_XYZ2( ix2, iy2, iz2 );
        *p_data++ = GS_XYZ2;

        *p_data++ = GS_SETREG_XYZ2( ix3, iy3, iz3 );
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
        
        for(count = 0; count < (segments * 2); count+=2)
        {
                vertexdata[count] = gsKit_scale(gsGlobal, GS_AXIS_X, *TriStrip++);
                vertexdata[count+1] = gsKit_scale(gsGlobal, GS_AXIS_Y, *TriStrip++);
        }

	int iz = gsKit_scale(gsGlobal, GS_AXIS_Z, z);

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

void gsKit_prim_triangle_strip_3d(GSGLOBAL *gsGlobal, float *TriStrip, int segments, u64 color)
{
        u64* p_store;
        u64* p_data;
        int size = 3 + segments;
        int count;
        int vertexdata[segments*3];
        
        for(count = 0; count < (segments * 3); count+=3)
        {
                vertexdata[count] = gsKit_scale(gsGlobal, GS_AXIS_X, *TriStrip++);
                vertexdata[count+1] = gsKit_scale(gsGlobal, GS_AXIS_Y, *TriStrip++);
		vertexdata[count+2] = gsKit_scale(gsGlobal, GS_AXIS_Z, *TriStrip++);
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

        for(count = 0; count < (segments * 3); count+=3)
        {
                *p_data++ = GS_SETREG_XYZ2( vertexdata[count], vertexdata[count+1], vertexdata[count+2] );
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

        for(count = 0; count < (verticies * 2); count+=2)
        {
                vertexdata[count] = gsKit_scale(gsGlobal, GS_AXIS_X, *TriFan++);
                vertexdata[count+1] = gsKit_scale(gsGlobal, GS_AXIS_Y, *TriFan++);
        }

	int iz = gsKit_scale(gsGlobal, GS_AXIS_Z, z);

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

void gsKit_prim_triangle_fan_3d(GSGLOBAL *gsGlobal, float *TriFan, int verticies, u64 color)
{
        u64* p_store;
        u64* p_data;
        int size = 3 + verticies;
        int count;
        int vertexdata[verticies*3];

        for(count = 0; count < (verticies * 3); count+=3)
        {
                vertexdata[count] = gsKit_scale(gsGlobal, GS_AXIS_X, *TriFan++);
                vertexdata[count+1] = gsKit_scale(gsGlobal, GS_AXIS_Y, *TriFan++);
		vertexdata[count+2] = gsKit_scale(gsGlobal, GS_AXIS_Z, *TriFan++);
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

        for(count = 0; count < (verticies * 3); count+=3)
        {
                *p_data++ = GS_SETREG_XYZ2( vertexdata[count], vertexdata[count+1], vertexdata[count+2] );
                *p_data++ = GS_XYZ2;
        }

        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}


void gsKit_prim_triangle_gouraud_3d(GSGLOBAL *gsGlobal, float x1, float y1, float z1,
							float x2, float y2, float z2,
							float x3, float y3, float z3, 
							u64 color1, u64 color2, u64 color3)
{
	u64* p_store;
	u64* p_data;
	int size = 8;

	int ix1 = gsKit_scale(gsGlobal, GS_AXIS_X, x1);
	int ix2 = gsKit_scale(gsGlobal, GS_AXIS_X, x2);
	int ix3 = gsKit_scale(gsGlobal, GS_AXIS_X, x3);

	int iy1 = gsKit_scale(gsGlobal, GS_AXIS_Y, y1);
	int iy2 = gsKit_scale(gsGlobal, GS_AXIS_Y, y2);
	int iy3 = gsKit_scale(gsGlobal, GS_AXIS_Y, y3);

	int iz1 = gsKit_scale(gsGlobal, GS_AXIS_Z, z1);
	int iz2 = gsKit_scale(gsGlobal, GS_AXIS_Z, z2);
	int iz3 = gsKit_scale(gsGlobal, GS_AXIS_Z, z3);

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

        *p_data++ = GS_SETREG_XYZ2( ix1, iy1, iz1 );   
        *p_data++ = GS_XYZ2;
  
        *p_data++ = color2;
        *p_data++ = GS_RGBAQ;

        *p_data++ = GS_SETREG_XYZ2( ix2, iy2, iz2 );
        *p_data++ = GS_XYZ2;

        *p_data++ = color3;
        *p_data++ = GS_RGBAQ;

        *p_data++ = GS_SETREG_XYZ2( ix3, iy3, iz3 );
        *p_data++ = GS_XYZ2;
        
        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}

void gsKit_prim_quad_3d(GSGLOBAL *gsGlobal, float x1, float y1, float z1,
					    float x2, float y2, float z2,
					    float x3, float y3, float z3,
					    float x4, float y4, float z4, u64 color)
{
	u64* p_store;
	u64* p_data;
	int size = 7;
        
	int ix1 = gsKit_scale(gsGlobal, GS_AXIS_X, x1);
	int ix2 = gsKit_scale(gsGlobal, GS_AXIS_X, x2);
	int ix3 = gsKit_scale(gsGlobal, GS_AXIS_X, x3);
	int ix4 = gsKit_scale(gsGlobal, GS_AXIS_X, x4);
                
	int iy1 = gsKit_scale(gsGlobal, GS_AXIS_Y, y1);
	int iy2 = gsKit_scale(gsGlobal, GS_AXIS_Y, y2);
	int iy3 = gsKit_scale(gsGlobal, GS_AXIS_Y, y3);
	int iy4 = gsKit_scale(gsGlobal, GS_AXIS_Y, y4);

	int iz1 = gsKit_scale(gsGlobal, GS_AXIS_Z, z1);
	int iz2 = gsKit_scale(gsGlobal, GS_AXIS_Z, z2);
	int iz3 = gsKit_scale(gsGlobal, GS_AXIS_Z, z3);
	int iz4 = gsKit_scale(gsGlobal, GS_AXIS_Z, z4);
        
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

        *p_data++ = GS_SETREG_XYZ2( ix1, iy1, iz1 );
        *p_data++ = GS_XYZ2;

        *p_data++ = GS_SETREG_XYZ2( ix2, iy2, iz2 );
        *p_data++ = GS_XYZ2;

        *p_data++ = GS_SETREG_XYZ2( ix3, iy3, iz3 );
        *p_data++ = GS_XYZ2;

        *p_data++ = GS_SETREG_XYZ2( ix4, iy4, iz4 );
        *p_data++ = GS_XYZ2;

        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}


void gsKit_prim_quad_gouraud_3d(GSGLOBAL *gsGlobal, float x1, float y1, float z1,
						 float x2, float y2, float z2,
						 float x3, float y3, float z3,
						 float x4, float y4, float z4, 
						 u64 color1, u64 color2,
						 u64 color3, u64 color4)
{
	u64* p_store;
	u64* p_data;
	int size = 10;

	int ix1 = gsKit_scale(gsGlobal, GS_AXIS_X, x1);
	int ix2 = gsKit_scale(gsGlobal, GS_AXIS_X, x2);
	int ix3 = gsKit_scale(gsGlobal, GS_AXIS_X, x3);
	int ix4 = gsKit_scale(gsGlobal, GS_AXIS_X, x4);

	int iy1 = gsKit_scale(gsGlobal, GS_AXIS_Y, y1);
	int iy2 = gsKit_scale(gsGlobal, GS_AXIS_Y, y2);
	int iy3 = gsKit_scale(gsGlobal, GS_AXIS_Y, y3);
	int iy4 = gsKit_scale(gsGlobal, GS_AXIS_Y, y4);

	int iz1 = gsKit_scale(gsGlobal, GS_AXIS_Z, z1);
	int iz2 = gsKit_scale(gsGlobal, GS_AXIS_Z, z2);
	int iz3 = gsKit_scale(gsGlobal, GS_AXIS_Z, z3);
	int iz4 = gsKit_scale(gsGlobal, GS_AXIS_Z, z4);

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

	*p_data++ = GS_SETREG_XYZ2( ix1, iy1, iz1 );
	*p_data++ = GS_XYZ2;
	
	*p_data++ = color2;
	*p_data++ = GS_RGBAQ;
	
	*p_data++ = GS_SETREG_XYZ2( ix2, iy2, iz2 );
	*p_data++ = GS_XYZ2;

	*p_data++ = color3;
	*p_data++ = GS_RGBAQ;

	*p_data++ = GS_SETREG_XYZ2( ix3, iy3, iz3 );
	*p_data++ = GS_XYZ2;

	*p_data++ = color4;
	*p_data++ = GS_RGBAQ;

	*p_data++ = GS_SETREG_XYZ2( ix4, iy4, iz4 );
	*p_data++ = GS_XYZ2;

	dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}

void gsKit_zblank(GSGLOBAL *gsGlobal)
{
        u64* p_store;
        u64* p_data;

        int ix1 = gsKit_scale(gsGlobal, GS_AXIS_X, 0);
        int ix2 = gsKit_scale(gsGlobal, GS_AXIS_X, gsGlobal->Width);
        int iy1 = gsKit_scale(gsGlobal, GS_AXIS_Y, 0);
        int iy2 = gsKit_scale(gsGlobal, GS_AXIS_Y, gsGlobal->Height);
	int iz = 0;	

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

        *p_data++ = GS_SETREG_XYZ2( ix1, iy1, iz );
        *p_data++ = GS_XYZ2;

        *p_data++ = GS_SETREG_XYZ2( ix2, iy2, iz );
        *p_data++ = GS_XYZ2;

        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, 6 );
}

