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
#include "gsInline.h"

void gsKit_prim_point(GSGLOBAL *gsGlobal, float x, float y, int iz, u64 color)
{
	u64* p_store;
	u64* p_data;
	int qsize = 2;
	int bsize = 32;

	int ix = gsKit_float_to_int_x(gsGlobal, x);
	int iy = gsKit_float_to_int_y(gsGlobal, y);

	p_store = p_data = gsKit_heap_alloc(gsGlobal, qsize, bsize, GIF_PRIM_POINT);

	if(p_store == gsGlobal->CurQueue->last_tag)
	{
		*p_data++ = GIF_TAG_POINT(0);
		*p_data++ = GIF_TAG_POINT_REGS;
	}

	*p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_POINT, 0, 0, gsGlobal->PrimFogEnable,
					gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
					0, gsGlobal->PrimContext, 0) ;

	*p_data++ = color;
	*p_data++ = GS_SETREG_XYZ2( ix, iy, iz );
}

void gsKit_prim_line_3d(GSGLOBAL *gsGlobal, float x1, float y1, int iz1, float x2, float y2, int iz2, u64 color)
{
	u64* p_store;
	u64* p_data;
	int qsize = 2;
	int bsize = 32;

	int ix1 = gsKit_float_to_int_x(gsGlobal, x1);
	int iy1 = gsKit_float_to_int_y(gsGlobal, y1);

	int ix2 = gsKit_float_to_int_x(gsGlobal, x2);
	int iy2 = gsKit_float_to_int_y(gsGlobal, y2);

	p_store = p_data = gsKit_heap_alloc(gsGlobal, qsize, bsize, GIF_PRIM_LINE);

	if(p_store == gsGlobal->CurQueue->last_tag)
	{
		*p_data++ = GIF_TAG_LINE(0);
		*p_data++ = GIF_TAG_LINE_REGS;
	}

	*p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_LINE, 0, 0, gsGlobal->PrimFogEnable,
				gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
				0, gsGlobal->PrimContext, 0) ;

	*p_data++ = color;
	*p_data++ = GS_SETREG_XYZ2( ix1, iy1, iz1 );
	*p_data++ = GS_SETREG_XYZ2( ix2, iy2, iz2 );
}

void gsKit_prim_line_goraud_3d(GSGLOBAL *gsGlobal, float x1, float y1, int iz1, float x2, float y2, int iz2, u64 color1, u64 color2)
{
	u64* p_store;
	u64* p_data;
	int qsize = 3;
	int bsize = 48;

	int ix1 = gsKit_float_to_int_x(gsGlobal, x1);
	int iy1 = gsKit_float_to_int_y(gsGlobal, y1);

	int ix2 = gsKit_float_to_int_x(gsGlobal, x2);
	int iy2 = gsKit_float_to_int_y(gsGlobal, y2);

	p_store = p_data = gsKit_heap_alloc(gsGlobal, qsize, bsize, GIF_PRIM_LINE);

	if(p_store == gsGlobal->CurQueue->last_tag)
	{
		*p_data++ = GIF_TAG_LINE_GORAUD(0);
		*p_data++ = GIF_TAG_LINE_GORAUD_REGS;
	}

	*p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_LINE, 1, 0, gsGlobal->PrimFogEnable,
				gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
				0, gsGlobal->PrimContext, 0) ;

	*p_data++ = color1;
	*p_data++ = GS_SETREG_XYZ2( ix1, iy1, iz1 );

	*p_data++ = color2;
	*p_data++ = GS_SETREG_XYZ2( ix2, iy2, iz2 );
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
		vertexdata[count+0] = gsKit_float_to_int_x(gsGlobal, *LineStrip++);
		vertexdata[count+1] = gsKit_float_to_int_y(gsGlobal, *LineStrip++);
	}

	p_store = p_data = gsKit_heap_alloc(gsGlobal, qsize, (qsize * 16), GIF_AD);

	*p_data++ = GIF_TAG_AD(qsize);
	*p_data++ = GIF_AD;

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
		vertexdata[count+0] = gsKit_float_to_int_x(gsGlobal, *LineStrip++);
		vertexdata[count+1] = gsKit_float_to_int_y(gsGlobal, *LineStrip++);
		vertexdata[count+2] = (int)((*LineStrip++) * 16.0f); // z
	}

	p_store = p_data = gsKit_heap_alloc(gsGlobal, qsize, (qsize * 16), GIF_AD);

	*p_data++ = GIF_TAG_AD(qsize);
	*p_data++ = GIF_AD;

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
	int qsize = 2;
	int bsize = 32;

	int ix1 = gsKit_float_to_int_x(gsGlobal, x1);
	int iy1 = gsKit_float_to_int_y(gsGlobal, y1);

	int ix2 = gsKit_float_to_int_x(gsGlobal, x2);
	int iy2 = gsKit_float_to_int_y(gsGlobal, y2);

	p_store = p_data = gsKit_heap_alloc(gsGlobal, qsize, bsize, GIF_PRIM_SPRITE);

	if(p_store == gsGlobal->CurQueue->last_tag)
	{
		*p_data++ = GIF_TAG_SPRITE(0);
		*p_data++ = GIF_TAG_SPRITE_REGS;
	}

	*p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_SPRITE, 0, 0, gsGlobal->PrimFogEnable,
				gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
				0, gsGlobal->PrimContext, 0) ;

	*p_data++ = color;
	*p_data++ = GS_SETREG_XYZ2( ix1, iy1, iz );
	*p_data++ = GS_SETREG_XYZ2( ix2, iy2, iz );
}

void gsKit_prim_triangle_3d(GSGLOBAL *gsGlobal, float x1, float y1, int iz1,
						float x2, float y2, int iz2,
						float x3, float y3, int iz3, u64 color)
{
	u64* p_store;
	u64* p_data;
	int qsize = 3;
	int bsize = 48;

	int ix1 = gsKit_float_to_int_x(gsGlobal, x1);
	int iy1 = gsKit_float_to_int_y(gsGlobal, y1);

	int ix2 = gsKit_float_to_int_x(gsGlobal, x2);
	int iy2 = gsKit_float_to_int_y(gsGlobal, y2);

	int ix3 = gsKit_float_to_int_x(gsGlobal, x3);
	int iy3 = gsKit_float_to_int_y(gsGlobal, y3);

	p_store = p_data = gsKit_heap_alloc(gsGlobal, qsize, bsize, GIF_PRIM_TRIANGLE);

	*p_data++ = GIF_TAG_TRIANGLE(0);
	*p_data++ = GIF_TAG_TRIANGLE_REGS;

	*p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_TRIANGLE, 0, 0, gsGlobal->PrimFogEnable,
				gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
				0, gsGlobal->PrimContext, 0) ;

	*p_data++ = color;
	*p_data++ = GS_SETREG_XYZ2( ix1, iy1, iz1 );
	*p_data++ = GS_SETREG_XYZ2( ix2, iy2, iz2 );
	*p_data++ = GS_SETREG_XYZ2( ix3, iy3, iz3 );
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
		vertexdata[count+0] = gsKit_float_to_int_x(gsGlobal, *TriStrip++);
		vertexdata[count+1] = gsKit_float_to_int_y(gsGlobal, *TriStrip++);
	}

	p_store = p_data = gsKit_heap_alloc(gsGlobal, qsize, (qsize * 16), GIF_AD);

	*p_data++ = GIF_TAG_AD(qsize);
	*p_data++ = GIF_AD;

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
		vertexdata[count+0] = gsKit_float_to_int_x(gsGlobal, *TriStrip++);
		vertexdata[count+1] = gsKit_float_to_int_y(gsGlobal, *TriStrip++);
		vertexdata[count+2] = (int)((*TriStrip++) * 16.0f); // z
	}

	p_store = p_data = gsKit_heap_alloc(gsGlobal, qsize, (qsize * 16), GIF_AD);

	*p_data++ = GIF_TAG_AD(qsize);
	*p_data++ = GIF_AD;

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
		vertexdata[count+0] = gsKit_float_to_int_x(gsGlobal, *TriFan++);
		vertexdata[count+1] = gsKit_float_to_int_y(gsGlobal, *TriFan++);
	}

	p_store = p_data = gsKit_heap_alloc(gsGlobal, qsize, (qsize * 16), GIF_AD);

	*p_data++ = GIF_TAG_AD(qsize);
	*p_data++ = GIF_AD;

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
		vertexdata[count+0] = gsKit_float_to_int_x(gsGlobal, *TriFan++);
		vertexdata[count+1] = gsKit_float_to_int_y(gsGlobal, *TriFan++);
		vertexdata[count+2] = (int)(*TriFan++ * 16.0f); // z
	}

	p_store = p_data = gsKit_heap_alloc(gsGlobal, qsize, (qsize * 16), GIF_AD);

	*p_data++ = GIF_TAG_AD(qsize);
	*p_data++ = GIF_AD;

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
	int qsize = 4;
	int bsize = 64;

	int ix1 = gsKit_float_to_int_x(gsGlobal, x1);
	int iy1 = gsKit_float_to_int_y(gsGlobal, y1);

	int ix2 = gsKit_float_to_int_x(gsGlobal, x2);
	int iy2 = gsKit_float_to_int_y(gsGlobal, y2);

	int ix3 = gsKit_float_to_int_x(gsGlobal, x3);
	int iy3 = gsKit_float_to_int_y(gsGlobal, y3);

	p_store = p_data = gsKit_heap_alloc(gsGlobal, qsize, bsize, GIF_PRIM_TRIANGLE_GOURAUD);

	if(p_store == gsGlobal->CurQueue->last_tag)
	{
		*p_data++ = GIF_TAG_TRIANGLE_GOURAUD(0);
		*p_data++ = GIF_TAG_TRIANGLE_GOURAUD_REGS;
	}

	*p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_TRIANGLE, 1, 0, gsGlobal->PrimFogEnable,
				gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
				0, gsGlobal->PrimContext, 0) ;

	*p_data++ = color1;
	*p_data++ = GS_SETREG_XYZ2(ix1, iy1, iz1);

	*p_data++ = color2;
	*p_data++ = GS_SETREG_XYZ2(ix2, iy2, iz2);

	*p_data++ = color3;
	*p_data++ = GS_SETREG_XYZ2(ix3, iy3, iz3);
}

void gsKit_prim_quad_3d(GSGLOBAL *gsGlobal, float x1, float y1, int iz1,
					float x2, float y2, int iz2,
					float x3, float y3, int iz3,
					float x4, float y4, int iz4, u64 color)
{
	u64* p_store;
	u64* p_data;
	int qsize = 3;
	int bsize = 48;

	int ix1 = gsKit_float_to_int_x(gsGlobal, x1);
	int iy1 = gsKit_float_to_int_y(gsGlobal, y1);

	int ix2 = gsKit_float_to_int_x(gsGlobal, x2);
	int iy2 = gsKit_float_to_int_y(gsGlobal, y2);

	int ix3 = gsKit_float_to_int_x(gsGlobal, x3);
	int iy3 = gsKit_float_to_int_y(gsGlobal, y3);

	int ix4 = gsKit_float_to_int_x(gsGlobal, x4);
	int iy4 = gsKit_float_to_int_y(gsGlobal, y4);

	p_store = p_data = gsKit_heap_alloc( gsGlobal, qsize, bsize, GIF_PRIM_QUAD);

	if(p_store == gsGlobal->CurQueue->last_tag)
	{
		*p_data++ = GIF_TAG_QUAD(0);
		*p_data++ = GIF_TAG_QUAD_REGS;
	}

	*p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_TRISTRIP, 0, 0, gsGlobal->PrimFogEnable,
				gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
				0, gsGlobal->PrimContext, 0) ;

	*p_data++ = color;
	*p_data++ = GS_SETREG_XYZ2( ix1, iy1, iz1 );
	*p_data++ = GS_SETREG_XYZ2( ix2, iy2, iz2 );
	*p_data++ = GS_SETREG_XYZ2( ix3, iy3, iz3 );
	*p_data++ = GS_SETREG_XYZ2( ix4, iy4, iz4 );
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
	int qsize = 5;
	int bsize = 80;

	int ix1 = gsKit_float_to_int_x(gsGlobal, x1);
	int iy1 = gsKit_float_to_int_y(gsGlobal, y1);

	int ix2 = gsKit_float_to_int_x(gsGlobal, x2);
	int iy2 = gsKit_float_to_int_y(gsGlobal, y2);

	int ix3 = gsKit_float_to_int_x(gsGlobal, x3);
	int iy3 = gsKit_float_to_int_y(gsGlobal, y3);

	int ix4 = gsKit_float_to_int_x(gsGlobal, x4);
	int iy4 = gsKit_float_to_int_y(gsGlobal, y4);

	p_store = p_data = gsKit_heap_alloc(gsGlobal, qsize, bsize, GIF_PRIM_QUAD_GOURAUD);

	if(p_store == gsGlobal->CurQueue->last_tag)
	{
		*p_data++ = GIF_TAG_QUAD_GOURAUD(0);
		*p_data++ = GIF_TAG_QUAD_GOURAUD_REGS;
	}

	*p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_TRISTRIP, 1, 0, gsGlobal->PrimFogEnable,
				gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
				0, gsGlobal->PrimContext, 0) ;

	*p_data++ = color1;
	*p_data++ = GS_SETREG_XYZ2( ix1, iy1, iz1 );

	*p_data++ = color2;
	*p_data++ = GS_SETREG_XYZ2( ix2, iy2, iz2 );

	*p_data++ = color3;
	*p_data++ = GS_SETREG_XYZ2( ix3, iy3, iz3 );

	*p_data++ = color4;
	*p_data++ = GS_SETREG_XYZ2( ix4, iy4, iz4 );
}

