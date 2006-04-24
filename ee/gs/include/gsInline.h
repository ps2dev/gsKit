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
		gsGlobal->CurQueue->pool_cur += 16;
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
	gsGlobal->CurQueue->pool_cur += bsize;
	
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
	gsGlobal->CurQueue->pool_cur += bsize;
	gsGlobal->CurQueue->dma_tag = gsGlobal->CurQueue->pool_cur;
	gsGlobal->CurQueue->pool_cur += 16;
	
	return p_heap;
}

#endif /* __GSINLINE_H__ */
