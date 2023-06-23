//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsInline.h - Static Inline Routines
//
//

#ifndef __GSINLINE_H__
#define __GSINLINE_H__

#include <stdio.h>
#include <stdint.h>

//  Drawbuffer Heap Allocator, GSQUEUE version
static inline void *_gsKit_heap_alloc(GSQUEUE *q, int qsize, int bsize, int type)
{
	void *p_heap;

#ifdef GSKIT_DEBUG
	if(((u32)q->pool_cur + bsize ) >= (u32)q->pool_max[q->dbuf])
	{
		printf("GSKIT: WARNING! HEAP OVERFLOW FOR RENDERQUEUE %p!!\n", q);
		return NULL;
	}
#endif

	if((q->tag_size + qsize) >= 65535)
	{
		*(u64 *)q->dma_tag = DMA_TAG(q->tag_size, 0, DMA_CNT, 0, 0, 0);
		q->tag_size = 0;
		q->dma_tag = q->pool_cur;
		q->pool_cur = (u8*)q->pool_cur + 16;
	}

	if(type == GIF_AD || type != q->last_type || q->same_obj >= GS_GIF_BLOCKSIZE)
	{
		if(q->last_type != GIF_RESERVED && q->last_type != GIF_AD)
			*(u64 *)q->last_tag = ((u64)q->same_obj | *(u64 *)q->last_tag);

		qsize ++;
		bsize += 16;
		q->last_tag = q->pool_cur;
		q->same_obj = 0;
	}

	q->same_obj++;
	q->last_type = type;
	q->tag_size += qsize;
	p_heap = q->pool_cur;
	q->pool_cur = (u8*)q->pool_cur + bsize;

	return p_heap;
}

///  Drawbuffer Heap Allocator
static inline void *gsKit_heap_alloc(GSGLOBAL *gsGlobal, int qsize, int bsize, int type)
{
	return _gsKit_heap_alloc(gsGlobal->CurQueue, qsize, bsize, type);
}

//  Drawbuffer Heap Allocator (For Injected DMA_TAGs), GSQUEUE version
static inline void *_gsKit_heap_alloc_dma(GSQUEUE *q, int qsize, int bsize)
{
	void *p_heap;

#ifdef GSKIT_DEBUG
	if(((u32)q->pool_cur + bsize ) >= (u32)q->pool_max[q->dbuf])
	{
		printf("GSKIT: WARNING! HEAP OVERFLOW FOR RENDERQUEUE %p!!\n", q);
		return NULL;
	}
#endif

	if(q->last_type != GIF_RESERVED && q->last_type != GIF_AD)
	{
		*(u64 *)q->last_tag = ((u64)q->same_obj | *(u64 *)q->last_tag);
	}

	*(u64 *)q->dma_tag = DMA_TAG(q->tag_size, 0, DMA_CNT, 0, 0, 0);
	q->tag_size = 0;

	q->last_type = GIF_RESERVED;
	q->same_obj = 0;

	p_heap = q->pool_cur;
	q->pool_cur = (u8*)q->pool_cur + bsize;
	q->dma_tag = q->pool_cur;
	q->pool_cur = (u8*)q->pool_cur + 16;

	return p_heap;
}

///  Drawbuffer Heap Allocator (For Injected DMA_TAGs)
static inline void *gsKit_heap_alloc_dma(GSGLOBAL *gsGlobal, int qsize, int bsize)
{
	return _gsKit_heap_alloc_dma(gsGlobal->CurQueue, qsize, bsize);
}

static inline int __gsKit_float_to_int_uv(float fuv, int imax)
{
	int iuv = (int)(fuv * 16.0f);

	// Limit u/v to texture width/height

	if (iuv < 0)
		iuv = 0;

	if (iuv > imax)
		iuv = imax;

	// Prevent overflow when using maximum size texture

	if (iuv >= (1024 * 16))
		iuv = (1024 * 16) - 1;

	return iuv;
}

static inline int gsKit_float_to_int_u(const GSTEXTURE *Texture, float fu)
{
	return __gsKit_float_to_int_uv(fu, Texture->Width * 16);
}

static inline int gsKit_float_to_int_v(const GSTEXTURE *Texture, float fv)
{
	return __gsKit_float_to_int_uv(fv, Texture->Height * 16);
}

static inline int __gsKit_float_to_int_xy(float fxy, int offset)
{
	int ixy = (int)(fxy * 16.0f) + offset;

	// Limit x/y to Primitive Coordinate System (0 to 4095.9375)

	if (ixy < 0)
		ixy = 0;

	if (ixy >= (4096 * 16))
		ixy = (4096 * 16) - 1;

	return ixy;
}

static inline int gsKit_float_to_int_x(const GSGLOBAL *gsGlobal, float fx)
{
	return __gsKit_float_to_int_xy(fx, gsGlobal->OffsetX);
}

static inline int gsKit_float_to_int_y(const GSGLOBAL *gsGlobal, float fy)
{
	return __gsKit_float_to_int_xy(fy, gsGlobal->OffsetY);
}

static inline gs_xyz2 vertex_to_XYZ2(const GSGLOBAL *gsGlobal, float fx, float fy, int iz)
{
	gs_xyz2 res;

	res.xyz.x = gsKit_float_to_int_x(gsGlobal, fx);
	res.xyz.y = gsKit_float_to_int_y(gsGlobal, fy);
	res.xyz.z = iz;
	res.tag = GS_XYZ2;

	return res;
}

// If STQ coordinates are used set q to 1.0f otherwise keep it as 0.0f
static inline gs_rgbaq color_to_RGBAQ(uint8_t r, uint8_t g, uint8_t b, uint8_t a, float q)
{
	gs_rgbaq res;

	res.color.r = r;
	res.color.g = g;
	res.color.b = b;
	res.color.a = a;
	res.color.q = q;
	res.tag = GS_RGBAQ;

	return res;
}

static inline gs_uv vertex_to_UV(const GSTEXTURE *Texture, float u, float v)
{
	gs_uv res;

	int iu = gsKit_float_to_int_u(Texture, u);
	int iv = gsKit_float_to_int_v(Texture, v);

	res.coord = GS_SETREG_UV(iu, iv);
	res.tag = GS_UV;

	return res;
}

static inline gs_stq vertex_to_STQ(float s, float t)
{
	gs_stq res;

	res.st.s = s;
	res.st.t = t;
	res.tag = GS_ST;

	return res;
}

#endif /* __GSINLINE_H__ */
