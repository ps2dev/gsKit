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

static inline void *gsKit_spr_alloc(GSGLOBAL *gsGlobal, int qsize, int bsize)
{
        void *p_spr;
                 
        if( ((u32)gsGlobal->CurQueue->spr_cur + bsize ) >= 0x70004000)
        {
                gsKit_kick_spr(gsGlobal, qsize);
        }

        p_spr = gsGlobal->CurQueue->spr_cur;
        gsGlobal->CurQueue->spr_cur += bsize;
        gsGlobal->CurQueue->size += qsize;

        return p_spr;
}

void gsKit_prim_point(GSGLOBAL *gsGlobal, float x, float y, int iz, u64 color)
{
        u64* p_store;
        u64* p_data;
        int qsize = 3;
        int bsize = 48;
	
        if(gsGlobal->Field == GS_FRAME)
        {
                y /= 2;
#ifdef GSKIT_ENABLE_HBOFFSET
                if(!gsGlobal->EvenOrOdd)
                {
                        y += 0.5;
                }
#endif
        }
        
        int ix = (int)(x * 16.0f) + gsGlobal->Offset;
        int iy = (int)(y * 16.0f) + gsGlobal->Offset;

        if( gsGlobal->PrimAlphaEnable == 1 )
	{
                qsize++;
		bsize += 16;
	}

        p_store = p_data = gsKit_spr_alloc( gsGlobal, qsize, bsize);

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

}

void gsKit_prim_line_3d(GSGLOBAL *gsGlobal, float x1, float y1, int iz1, float x2, float y2, int iz2, u64 color)
{
        u64* p_store;
        u64* p_data;
        int qsize = 4;
        int bsize = 64;

        if(gsGlobal->Field == GS_FRAME)
        {
                y1 /= 2;
                y2 /= 2;
#ifdef GSKIT_ENABLE_HBOFFSET
                if(!gsGlobal->EvenOrOdd)
                {
                        y1 += 0.5;
                        y2 += 0.5;
                }
#endif
        }
        
        
        int ix1 = (int)(x1 * 16.0f) + gsGlobal->Offset;
        int iy1 = (int)(y1 * 16.0f) + gsGlobal->Offset;
        
        int ix2 = (int)(x2 * 16.0f) + gsGlobal->Offset;
        int iy2 = (int)(y2 * 16.0f) + gsGlobal->Offset;
         
        if( gsGlobal->PrimAlphaEnable == 1 )
	{
                qsize++;
		bsize += 16;
	}

        p_store = p_data = gsKit_spr_alloc( gsGlobal, qsize, bsize);


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

}

void gsKit_prim_line_strip(GSGLOBAL *gsGlobal, float *LineStrip, int segments, int iz, u64 color)
{
        u64* p_store;
        u64* p_data;
        int qsize = 2 + segments;
	int count;
	int vertexdata[segments*2];

	for(count = 0; count < (segments * 2); count+=2)
	{
	        vertexdata[count] = (int)((*LineStrip++) * 16.0f) + gsGlobal->Offset;
		if(gsGlobal->Field == GS_FRAME)
	        {
			*(LineStrip) /= 2;
		#ifdef GSKIT_ENABLE_HBOFFSET
	                if(!gsGlobal->EvenOrOdd)
	                {
				*(LineStrip) += 0.5;
	                }
		#endif
		}
	        vertexdata[count+1] = (int)((*LineStrip++) * 16.0f) + gsGlobal->Offset;
	}

        if( gsGlobal->PrimAlphaEnable == 1 )
                qsize++;
        
        p_store = p_data = gsKit_spr_alloc( gsGlobal, qsize, (qsize * 16));

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

}

void gsKit_prim_line_strip_3d(GSGLOBAL *gsGlobal, float *LineStrip, int segments, u64 color)
{
        u64* p_store;
        u64* p_data;
        int qsize = 2 + segments;
	int count;
	int vertexdata[segments*3];

	for(count = 0; count < (segments * 3); count+=3)
	{
	        vertexdata[count] = (int)((*LineStrip++) * 16.0f) + gsGlobal->Offset;
                if(gsGlobal->Field == GS_FRAME)
                {
                        *(LineStrip) /= 2;
                #ifdef GSKIT_ENABLE_HBOFFSET
                        if(!gsGlobal->EvenOrOdd)
                        {
	                        *(LineStrip) += 0.5;
                        }
                #endif
                }
	        vertexdata[count+1] = (int)((*LineStrip++) * 16.0f) + gsGlobal->Offset;
		vertexdata[count+2] = (int)((*LineStrip++) * 16.0f);
	}

        if( gsGlobal->PrimAlphaEnable == 1 )
                qsize++;
        
        p_store = p_data = gsKit_spr_alloc( gsGlobal, qsize, (qsize * 16));
        
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

}

void gsKit_prim_sprite(GSGLOBAL *gsGlobal, float x1, float y1, float x2, float y2, int iz, u64 color)
{
	u64* p_store;
	u64* p_data;
	int qsize = 4;
	int bsize = 64;	

        if(gsGlobal->Field == GS_FRAME)
        {
                y1 /= 2;
                y2 /= 2;
#ifdef GSKIT_ENABLE_HBOFFSET
                if(!gsGlobal->EvenOrOdd)
                {
                        y1 += 0.5;
                        y2 += 0.5;
                }
#endif
	}

        int ix1 = (int)(x1 * 16.0f) + gsGlobal->Offset;
        int ix2 = (int)(x2 * 16.0f) + gsGlobal->Offset;
        int iy1 = (int)(y1 * 16.0f) + gsGlobal->Offset;
        int iy2 = (int)(y2 * 16.0f) + gsGlobal->Offset;

        if( gsGlobal->PrimAlphaEnable == 1 )
	{
                qsize++;
		bsize += 16;
	}

        p_store = p_data = gsKit_spr_alloc( gsGlobal, qsize, bsize );

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

}

void gsKit_prim_triangle_3d(GSGLOBAL *gsGlobal, float x1, float y1, int iz1, 
						float x2, float y2, int iz2,
						float x3, float y3, int iz3, u64 color)
{
	u64* p_store;
	u64* p_data;
	int qsize = 5;
	int bsize = 80;

        if(gsGlobal->Field == GS_FRAME)
        {
                y1 /= 2;
                y2 /= 2;
                y3 /= 2;
#ifdef GSKIT_ENABLE_HBOFFSET
                if(!gsGlobal->EvenOrOdd)
                {
                        y1 += 0.5;
                        y2 += 0.5;
                        y3 += 0.5;
                }
#endif
        }
        
        
        int ix1 = (int)(x1 * 16.0f) + gsGlobal->Offset;
        int iy1 = (int)(y1 * 16.0f) + gsGlobal->Offset;
        
        int ix2 = (int)(x2 * 16.0f) + gsGlobal->Offset;
        int iy2 = (int)(y2 * 16.0f) + gsGlobal->Offset;
         
        int ix3 = (int)(x3 * 16.0f) + gsGlobal->Offset;
        int iy3 = (int)(y3 * 16.0f) + gsGlobal->Offset;

        if( gsGlobal->PrimAlphaEnable == 1 )
	{
                qsize++;
		bsize += 16;
	}

        p_store = p_data = gsKit_spr_alloc( gsGlobal, qsize, bsize );

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

}

void gsKit_prim_triangle_strip(GSGLOBAL *gsGlobal, float *TriStrip, int segments, int iz, u64 color)
{
        u64* p_store;
        u64* p_data;
        int qsize = 2 + segments;
        int count;
        int vertexdata[segments*2];
        
        for(count = 0; count < (segments * 2); count+=2)
        {
                vertexdata[count] = (int)((*TriStrip++) * 16.0f) + gsGlobal->Offset;
                if(gsGlobal->Field == GS_FRAME)
                {   
                        *(TriStrip) /= 2;
                #ifdef GSKIT_ENABLE_HBOFFSET
                        if(!gsGlobal->EvenOrOdd)
                        {
	                       *(TriStrip) += 0.5;
                        }
                #endif
                }
                vertexdata[count+1] = (int)((*TriStrip++) * 16.0f) + gsGlobal->Offset;
        }

        if( gsGlobal->PrimAlphaEnable == 1 )
                qsize++;

        p_store = p_data = gsKit_spr_alloc( gsGlobal, qsize, (qsize * 16));

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
        
}

void gsKit_prim_triangle_strip_3d(GSGLOBAL *gsGlobal, float *TriStrip, int segments, u64 color)
{
        u64* p_store;
        u64* p_data;
        int qsize = 2 + segments;
        int count;
        int vertexdata[segments*3];
        
        for(count = 0; count < (segments * 3); count+=3)
        {
                vertexdata[count] = (int)((*TriStrip++) * 16.0f) + gsGlobal->Offset;
                if(gsGlobal->Field == GS_FRAME)
                {   
                        *(TriStrip) /= 2;
                #ifdef GSKIT_ENABLE_HBOFFSET
                        if(!gsGlobal->EvenOrOdd)
                        {
                        *(TriStrip) += 0.5;
                        }
                #endif
                }
                vertexdata[count+1] = (int)((*TriStrip++) * 16.0f) + gsGlobal->Offset;
                vertexdata[count+2] = (int)((*TriStrip++) * 16.0f);
        }

        if( gsGlobal->PrimAlphaEnable == 1 )
                qsize++;

        p_store = p_data = gsKit_spr_alloc( gsGlobal, qsize, (qsize * 16) );

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
        
}

void gsKit_prim_triangle_fan(GSGLOBAL *gsGlobal, float *TriFan, int verticies, int iz, u64 color)
{
        u64* p_store;
        u64* p_data;
        int qsize = 2 + verticies;
        int count;
        int vertexdata[verticies*2];

        for(count = 0; count < (verticies * 2); count+=2)
        {
                vertexdata[count] = (int)((*TriFan++) * 16.0f) + gsGlobal->Offset;
                if(gsGlobal->Field == GS_FRAME)
                {   
                        *(TriFan) /= 2;
                #ifdef GSKIT_ENABLE_HBOFFSET
                        if(!gsGlobal->EvenOrOdd)
                        {
                        *(TriFan) += 0.5;
                        }
                #endif
                }
                vertexdata[count+1] = (int)((*TriFan++) * 16.0f) + gsGlobal->Offset;
        }

        if( gsGlobal->PrimAlphaEnable == 1 )
                qsize++;

        p_store = p_data = gsKit_spr_alloc( gsGlobal, qsize, (qsize * 16) );

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

}

void gsKit_prim_triangle_fan_3d(GSGLOBAL *gsGlobal, float *TriFan, int verticies, u64 color)
{
        u64* p_store;
        u64* p_data;
        int qsize = 2 + verticies;
        int count;
        int vertexdata[verticies*3];

        for(count = 0; count < (verticies * 3); count+=3)
        {
                vertexdata[count] = (int)((*TriFan++) * 16.0f) + gsGlobal->Offset;
                if(gsGlobal->Field == GS_FRAME)
                {
                        *(TriFan) /= 2;
                #ifdef GSKIT_ENABLE_HBOFFSET
                        if(!gsGlobal->EvenOrOdd)
                        {
                        *(TriFan) += 0.5;
                        }
                #endif
                }
                vertexdata[count+1] = (int)((*TriFan++) * 16.0f) + gsGlobal->Offset;
                vertexdata[count+2] = (int)(*TriFan++ * 16.0f);
        }

        if( gsGlobal->PrimAlphaEnable == 1 )
                qsize++;

        p_store = p_data = gsKit_spr_alloc( gsGlobal, qsize, (qsize * 16) );

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

}


void gsKit_prim_triangle_gouraud_3d(GSGLOBAL *gsGlobal, float x1, float y1, int iz1,
							float x2, float y2, int iz2,
							float x3, float y3, int iz3, 
							u64 color1, u64 color2, u64 color3)
{
	u64* p_store;
	u64* p_data;
	int qsize = 7;
	int bsize = 112;

	if(gsGlobal->Field == GS_FRAME)
	{
		y1 /= 2;
		y2 /= 2;
		y3 /= 2;
#ifdef GSKIT_ENABLE_HBOFFSET
		if(!gsGlobal->EvenOrOdd)
		{
			y1 += 0.5;
			y2 += 0.5;
			y3 += 0.5;
		}
#endif
	}

	int ix1 = (int)(x1 * 16.0f) + gsGlobal->Offset;
	int iy1 = (int)(y1 * 16.0f) + gsGlobal->Offset;

        int ix2 = (int)(x2 * 16.0f) + gsGlobal->Offset;
        int iy2 = (int)(y2 * 16.0f) + gsGlobal->Offset;

        int ix3 = (int)(x3 * 16.0f) + gsGlobal->Offset;
        int iy3 = (int)(y3 * 16.0f) + gsGlobal->Offset;

	if( gsGlobal->PrimAlphaEnable == 1 )
	{
		qsize++;
		bsize += 16;
	}

	p_store = p_data = gsKit_spr_alloc(gsGlobal, qsize, bsize);


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

        *p_data++ = GS_SETREG_XYZ2(ix1, iy1, iz1);
        *p_data++ = GS_XYZ2;

        *p_data++ = color2;
        *p_data++ = GS_RGBAQ;

        *p_data++ = GS_SETREG_XYZ2(ix2, iy2, iz2);
        *p_data++ = GS_XYZ2;

        *p_data++ = color3;
        *p_data++ = GS_RGBAQ;

        *p_data++ = GS_SETREG_XYZ2(ix3, iy3, iz3);
        *p_data++ = GS_XYZ2;
}

void gsKit_prim_quad_3d(GSGLOBAL *gsGlobal, float x1, float y1, int iz1,
					    float x2, float y2, int iz2,
					    float x3, float y3, int iz3,
					    float x4, float y4, int iz4, u64 color)
{
	u64* p_store;
	u64* p_data;
	int qsize = 6;
	int bsize = 96;
        
        if(gsGlobal->Field == GS_FRAME)
        {
                y1 /= 2;
                y2 /= 2;
                y3 /= 2;
                y4 /= 2;
#ifdef GSKIT_ENABLE_HBOFFSET
                if(!gsGlobal->EvenOrOdd)
                {
                        y1 += 0.5;
                        y2 += 0.5;
                        y3 += 0.5;
                        y4 += 0.5;
                }
#endif
        }

        int ix1 = (int)(x1 * 16.0f) + gsGlobal->Offset;
        int iy1 = (int)(y1 * 16.0f) + gsGlobal->Offset;

        int ix2 = (int)(x2 * 16.0f) + gsGlobal->Offset;
        int iy2 = (int)(y2 * 16.0f) + gsGlobal->Offset;

        int ix3 = (int)(x3 * 16.0f) + gsGlobal->Offset;
        int iy3 = (int)(y3 * 16.0f) + gsGlobal->Offset;

        int ix4 = (int)(x4 * 16.0f) + gsGlobal->Offset;
        int iy4 = (int)(y4 * 16.0f) + gsGlobal->Offset;

        if( gsGlobal->PrimAlphaEnable == 1 )
	{
        	qsize++;
		bsize += 16;
	}

        p_store = p_data = gsKit_spr_alloc( gsGlobal, qsize, bsize );

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

}


void gsKit_prim_quad_gouraud_3d(GSGLOBAL *gsGlobal, float x1, float y1, int iz1,
						 float x2, float y2, int iz2,
						 float x3, float y3, int iz3,
						 float x4, float y4, int iz4, 
						 u64 color1, u64 color2,
						 u64 color3, u64 color4)
{
	u64* p_store;
	u64* p_data;
	int qsize = 9;
	int bsize = 144;

        if(gsGlobal->Field == GS_FRAME)
        {
                y1 /= 2;
                y2 /= 2;
                y3 /= 2;
                y4 /= 2;
#ifdef GSKIT_ENABLE_HBOFFSET
                if(!gsGlobal->EvenOrOdd)
                {
                        y1 += 0.5;
                        y2 += 0.5;
                        y3 += 0.5;
                        y4 += 0.5;
                }
#endif
        }

        int ix1 = (int)(x1 * 16.0f) + gsGlobal->Offset;
        int iy1 = (int)(y1 * 16.0f) + gsGlobal->Offset;

        int ix2 = (int)(x2 * 16.0f) + gsGlobal->Offset;
        int iy2 = (int)(y2 * 16.0f) + gsGlobal->Offset;

        int ix3 = (int)(x3 * 16.0f) + gsGlobal->Offset;
        int iy3 = (int)(y3 * 16.0f) + gsGlobal->Offset;

        int ix4 = (int)(x4 * 16.0f) + gsGlobal->Offset;
        int iy4 = (int)(y4 * 16.0f) + gsGlobal->Offset;

	if( gsGlobal->PrimAlphaEnable == 1 )
	{
		qsize++;
		bsize += 16;
	}

	p_store = p_data = gsKit_spr_alloc( gsGlobal, qsize, bsize );
         
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

}

void gsKit_zblank(GSGLOBAL *gsGlobal)
{
        u64* p_store;
        u64* p_data;

        int ix1 = gsGlobal->Offset;
        int iy1 = gsGlobal->Offset;
        
        int ix2 = (int)(gsGlobal->Width * 16.0f) + gsGlobal->Offset;
        int iy2 = (int)(gsGlobal->Height * 16.0f) + gsGlobal->Offset;

	int iz = 0;	

        p_store = p_data = gsKit_spr_alloc( gsGlobal, 5, 80 );

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

}

