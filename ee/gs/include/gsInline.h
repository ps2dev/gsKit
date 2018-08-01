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

///  Drawbuffer Heap Allocator
static inline void *gsKit_heap_alloc(GSGLOBAL *gsGlobal, int qsize, int bsize, int type)
{
#ifdef GSKIT_DEBUG
	if(((u32)gsGlobal->CurQueue->pool_cur + bsize ) >= (u32)gsGlobal->CurQueue->pool_max[gsGlobal->CurQueue->dbuf])
	{
		printf("GSKIT: WARNING! HEAP OVERFLOW FOR RENDERQUEUE %p!!\n", gsGlobal->CurQueue);
		return NULL;
	}
#endif

	if((gsGlobal->CurQueue->tag_size + qsize) >= 65535)
	{
		*(u64 *)gsGlobal->CurQueue->dma_tag = DMA_TAG(gsGlobal->CurQueue->tag_size, 0, DMA_CNT, 0, 0, 0);
		gsGlobal->CurQueue->tag_size = 0;
		gsGlobal->CurQueue->dma_tag = gsGlobal->CurQueue->pool_cur;
		(u8*)gsGlobal->CurQueue->pool_cur += 16;
	}

	if(type == GIF_AD || type != gsGlobal->CurQueue->last_type || gsGlobal->CurQueue->same_obj >= GS_GIF_BLOCKSIZE)
	{
		if(gsGlobal->CurQueue->last_type != GIF_RESERVED && gsGlobal->CurQueue->last_type != GIF_AD)
		{
			*(u64 *)gsGlobal->CurQueue->last_tag = ((u64)gsGlobal->CurQueue->same_obj | *(u64 *)gsGlobal->CurQueue->last_tag);
		}

		qsize ++;
		bsize += 16;
		gsGlobal->CurQueue->last_tag = gsGlobal->CurQueue->pool_cur;
		gsGlobal->CurQueue->same_obj = 0;
	}

	gsGlobal->CurQueue->same_obj++;
	gsGlobal->CurQueue->last_type = type;
	gsGlobal->CurQueue->tag_size += qsize;
	void *p_heap = gsGlobal->CurQueue->pool_cur;
	(u8*)gsGlobal->CurQueue->pool_cur += bsize;

	return p_heap;
}

///  Drawbuffer Heap Allocator (For Injected DMA_TAGs)
static inline void *gsKit_heap_alloc_dma(GSGLOBAL *gsGlobal, int qsize, int bsize)
{
#ifdef GSKIT_DEBUG
	if(((u32)gsGlobal->CurQueue->pool_cur + bsize ) >= (u32)gsGlobal->CurQueue->pool_max[gsGlobal->CurQueue->dbuf])
	{
		printf("GSKIT: WARNING! HEAP OVERFLOW FOR RENDERQUEUE %p!!\n", gsGlobal->CurQueue);
		return NULL;
	}
#endif

	if(gsGlobal->CurQueue->last_type != GIF_RESERVED && gsGlobal->CurQueue->last_type != GIF_AD)
	{
		*(u64 *)gsGlobal->CurQueue->last_tag = ((u64)gsGlobal->CurQueue->same_obj | *(u64 *)gsGlobal->CurQueue->last_tag);
	}

	*(u64 *)gsGlobal->CurQueue->dma_tag = DMA_TAG(gsGlobal->CurQueue->tag_size, 0, DMA_CNT, 0, 0, 0);
	gsGlobal->CurQueue->tag_size = 0;

	gsGlobal->CurQueue->last_type = GIF_RESERVED;
	gsGlobal->CurQueue->same_obj = 0;

	void *p_heap = gsGlobal->CurQueue->pool_cur;
	(u8*)gsGlobal->CurQueue->pool_cur += bsize;
	gsGlobal->CurQueue->dma_tag = gsGlobal->CurQueue->pool_cur;
	(u8*)gsGlobal->CurQueue->pool_cur += 16;

	return p_heap;
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

#endif /* __GSINLINE_H__ */
