//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsCore.c - Core functionality implimentations. (Allocators, RenderStates, etc)
//
// Parts taken from emoon's BreakPoint Demo Library
//

#include "gsKit.h"
#include "gsInline.h"

#include <kernel.h>
#include <malloc.h>

#if F_gsKit_vram_alloc
u32 gsKit_vram_alloc(GSGLOBAL *gsGlobal, u32 size, u8 type)
{
	u32 CurrentPointer = gsGlobal->CurrentPointer;
	#ifdef GSKIT_DEBUG
		printf("CurrentPointer Before:\t0x%08X\n", gsGlobal->CurrentPointer);
	#endif

	if(type == GSKIT_ALLOC_SYSBUFFER)
		size = (-GS_VRAM_BLOCKSIZE_8K)&(size+GS_VRAM_BLOCKSIZE_8K-1);
	else
		size = (-GS_VRAM_BLOCKSIZE_256)&(size+GS_VRAM_BLOCKSIZE_256-1);

	if((gsGlobal->CurrentPointer + size)>= 4194304)
	{
		printf("ERROR: Not enough VRAM for this allocation!\n");
		return GSKIT_ALLOC_ERROR;
	}
	else
	{
		gsGlobal->CurrentPointer += size;

		// Re-initialize the texture manager
		// NOTE: this is here for compatibility, it's better not to use
		//       gsKit_vram_alloc, and use gsKit_TexManager_bind instead.
		gsKit_TexManager_init(gsGlobal);

		#ifdef GSKIT_DEBUG
		printf("CurrentPointer After:\t0x%08X\n", gsGlobal->CurrentPointer);
		#endif
		return CurrentPointer;
	}
}
#endif

#if F_gsKit_vram_clear
void gsKit_vram_clear(GSGLOBAL *gsGlobal)
{
    gsGlobal->CurrentPointer = gsGlobal->TexturePointer;

	// Re-initialize the texture manager
	// NOTE: this is here for compatibility, it's better not to use
	//       gsKit_vram_alloc, and use gsKit_TexManager_bind instead.
	gsKit_TexManager_init(gsGlobal);
}
#endif

#if F_gsKit_sync_flip
void gsKit_sync_flip(GSGLOBAL *gsGlobal)
{
	if(!gsGlobal->FirstFrame)
	{
		gsKit_vsync_wait();

		if(gsGlobal->DoubleBuffering == GS_SETTING_ON)
		{
			GS_SET_DISPFB2( gsGlobal->ScreenBuffer[gsGlobal->ActiveBuffer & 1] / 8192,
				gsGlobal->Width / 64, gsGlobal->PSM, 0, 0 );

			gsGlobal->ActiveBuffer ^= 1;
		}

	}

	gsKit_setactive(gsGlobal);

}
#endif

#if F_gsKit_setactive
void gsKit_setactive(GSGLOBAL *gsGlobal)
{
	u64 *p_data;
	u64 *p_store;

	p_data = p_store = (u64 *)gsGlobal->dma_misc;

	*p_data++ = GIF_TAG( 4, 1, 0, 0, GSKIT_GIF_FLG_PACKED, 1 );
	*p_data++ = GIF_AD;

	// Context 1

	*p_data++ = GS_SETREG_SCISSOR_1( 0, gsGlobal->Width - 1, 0, gsGlobal->Height - 1 );
	*p_data++ = GS_SCISSOR_1;

	*p_data++ = GS_SETREG_FRAME_1( gsGlobal->ScreenBuffer[gsGlobal->ActiveBuffer & 1] / 8192,
                                 gsGlobal->Width / 64, gsGlobal->PSM, 0 );
	*p_data++ = GS_FRAME_1;

	// Context 2

	*p_data++ = GS_SETREG_SCISSOR_1( 0, gsGlobal->Width - 1, 0, gsGlobal->Height - 1 );
	*p_data++ = GS_SCISSOR_2;

	*p_data++ = GS_SETREG_FRAME_1( gsGlobal->ScreenBuffer[gsGlobal->ActiveBuffer & 1] / 8192,
                                 gsGlobal->Width / 64, gsGlobal->PSM, 0 );
	*p_data++ = GS_FRAME_2;

	dmaKit_wait_fast();
	dmaKit_send_ucab(DMA_CHANNEL_GIF, p_store, 5);
}
#endif

#if F_gsKit_finish
void gsKit_finish(void)
{
	while(!(GS_CSR_FINISH));
}
#endif

#if F_gsKit_lock_buffer
void gsKit_lock_buffer(GSGLOBAL *gsGlobal)
{
    gsGlobal->LockBuffer = GS_SETTING_ON;
}
#endif

#if F_gsKit_unlock_buffer
void gsKit_unlock_buffer(GSGLOBAL *gsGlobal)
{
    gsGlobal->LockBuffer = GS_SETTING_OFF;
}
#endif

#if F_gsKit_lock_status
int gsKit_lock_status(GSGLOBAL *gsGlobal)
{
    return gsGlobal->LockBuffer;
}
#endif

#if F_gsKit_display_buffer
void gsKit_display_buffer(GSGLOBAL *gsGlobal)
{
    GS_SET_DISPFB2( gsGlobal->ScreenBuffer[gsGlobal->ActiveBuffer & 1] / 8192,
            gsGlobal->Width / 64, gsGlobal->PSM, 0, 0 );
}
#endif

#if F_gsKit_switch_context
void gsKit_switch_context(GSGLOBAL *gsGlobal)
{
    gsGlobal->ActiveBuffer ^= 1;
    gsGlobal->PrimContext ^= 1;

    gsKit_setactive(gsGlobal);
}
#endif

#if F_gsKit_vsync_wait
void gsKit_vsync_wait(void)
{
	*GS_CSR = *GS_CSR & 8;
	while(!(*GS_CSR & 8));
}
#endif

#if F_gsKit_vsync_nowait
void gsKit_vsync_nowait(void)
{
    *GS_CSR = *GS_CSR & 8;
}
#endif

#if F_gsKit_get_field
void gsKit_get_field(GSGLOBAL *gsGlobal)
{
    gsGlobal->EvenOrOdd=((GSREG*)GS_CSR)->FIELD;
}
#endif

#if F_gsKit_hsync_wait
void gsKit_hsync_wait(void)
{
    *GS_CSR = *GS_CSR & 4;
    while(!(*GS_CSR & 4));
}
#endif

#if F_gsKit_add_vsync_handler
int gsKit_add_vsync_handler(int (*vsync_callback)(int))
{
	int callback_id;

	DIntr();
	callback_id = AddIntcHandler(INTC_VBLANK_S, vsync_callback, 0);
	EnableIntc(INTC_VBLANK_S);
	// Unmask VSync interrupt
	GsPutIMR(GsGetIMR() & ~0x0800);
	EIntr();

	return callback_id;
}
#endif

#if F_gsKit_remove_vsync_handler
void gsKit_remove_vsync_handler(int callback_id)
{
	DIntr();
	// Mask VSync interrupt
	GsPutIMR(GsGetIMR() | 0x0800);
	DisableIntc(INTC_VBLANK_S);
	RemoveIntcHandler(INTC_VBLANK_S, callback_id);
	EIntr();
}
#endif

#if F_gsKit_add_hsync_handler
int gsKit_add_hsync_handler(int (*hsync_callback)(int))
{
	int callback_id;

	DIntr();
	callback_id = AddIntcHandler(INTC_GS, hsync_callback, 0);
	EnableIntc(INTC_GS);
	// Unmask HSync interrupt
	GsPutIMR(GsGetIMR() & ~0x0400);
	EIntr();

	return callback_id;
}
#endif

#if F_gsKit_remove_hsync_handler
void gsKit_remove_hsync_handler(int callback_id)
{
	DIntr();
	// Mask HSync interrupt
	GsPutIMR(GsGetIMR() | 0x0400);
	DisableIntc(INTC_GS);
	RemoveIntcHandler(INTC_GS, callback_id);
	EIntr();
}
#endif

#if F_gsKit_add_finish_handler
int gsKit_add_finish_handler(int (*finish_callback)(int))
{
	int callback_id;

	DIntr();
	callback_id = AddIntcHandler(INTC_GS, finish_callback, 0);
	EnableIntc(INTC_GS);
	// Unmask Finish interrupt
	GsPutIMR(GsGetIMR() & ~0x0200);
	EIntr();

	return callback_id;
}
#endif

#if F_gsKit_remove_finish_handler
void gsKit_remove_finish_handler(int callback_id)
{
	DIntr();
	// Mask HSync interrupt
	GsPutIMR(GsGetIMR() | 0x0200);
	DisableIntc(INTC_GS);
	RemoveIntcHandler(INTC_GS, callback_id);
	EIntr();
}
#endif

#if F_gsKit_clear
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
void gsKit_clear(GSGLOBAL *gsGlobal, u64 color)
{
	u8 PrevZState;
	u8 strips;
	u8 remain;
	u8 index;
	u32 pos;
	u8 slices = (gsGlobal->Width + 63)/ 64;
	u32 count = (slices * 2) + 1;
	u128 flat_content[count];

	PrevZState = gsGlobal->Test->ZTST;
	gsKit_set_test(gsGlobal, GS_ZTEST_OFF);

	flat_content[0] = (u128)rgbaq_to_RGBAQ(color).rgbaq;
	for (index = 0; index < slices; index++)
	{
		flat_content[index * 2 + 1] = vertex_to_XYZ2(gsGlobal, index * 64, 0, 0).xyz2;
		flat_content[index * 2 + 2] = (u128)vertex_to_XYZ2(gsGlobal, MIN((index + 1) * 64, gsGlobal->Width) , gsGlobal->Height, 0).xyz2;
	}
	gsKit_prim_list_sprite_flat(gsGlobal, count, flat_content);

	gsGlobal->Test->ZTST = PrevZState;
	gsKit_set_test(gsGlobal, 0);
}
#endif

#if F_gsKit_set_scissor
void gsKit_set_scissor(GSGLOBAL *gsGlobal, u64 ScissorBounds) {
    u64 *p_data;
	u64 *p_store;

	if (ScissorBounds == GS_SCISSOR_RESET)
		ScissorBounds = GS_SETREG_SCISSOR(0, gsGlobal->Width - 1, 0, gsGlobal->Height - 1);

	p_data = p_store = gsKit_heap_alloc(gsGlobal, 1, 16, GIF_AD);

	*p_data++ = GIF_TAG_AD(1);
	*p_data++ = GIF_AD;

	*p_data++ = ScissorBounds;
	*p_data++ = GS_SCISSOR_1+gsGlobal->PrimContext;
}
#endif

#if F_gsKit_set_test
void gsKit_set_test(GSGLOBAL *gsGlobal, u8 Preset)
{
	u64 *p_data;
	u64 *p_store;

	if(Preset == GS_ZTEST_OFF)
		gsGlobal->Test->ZTST = 1;
	else if (Preset == GS_ZTEST_ON)
		gsGlobal->Test->ZTST = 2;
	else if (Preset == GS_ATEST_OFF)
		gsGlobal->Test->ATE = 0;
	else if (Preset == GS_ATEST_ON)
		gsGlobal->Test->ATE = 1;
	else if (Preset == GS_D_ATEST_OFF)
		gsGlobal->Test->DATE = 0;
	else if (Preset == GS_D_ATEST_ON)
		gsGlobal->Test->DATE = 1;

	p_data = p_store = gsKit_heap_alloc(gsGlobal, 1, 16, GIF_AD);

	*p_data++ = GIF_TAG_AD(1);
	*p_data++ = GIF_AD;

	*p_data++ = GS_SETREG_TEST( gsGlobal->Test->ATE, gsGlobal->Test->ATST,
				    gsGlobal->Test->AREF, gsGlobal->Test->AFAIL,
				    gsGlobal->Test->DATE, gsGlobal->Test->DATM,
				    gsGlobal->Test->ZTE, gsGlobal->Test->ZTST );

	*p_data++ = GS_TEST_1+gsGlobal->PrimContext;

}
#endif

#if F_gsKit_set_clamp
void gsKit_set_clamp(GSGLOBAL *gsGlobal, u8 Preset)
{
	u64 *p_data;
	u64 *p_store;

	if(Preset == GS_CMODE_REPEAT)
	{
		gsGlobal->Clamp->WMS = GS_CMODE_REPEAT;
		gsGlobal->Clamp->WMT = GS_CMODE_REPEAT;
	}
	else if(Preset == GS_CMODE_CLAMP)
	{
		gsGlobal->Clamp->WMS = GS_CMODE_CLAMP;
		gsGlobal->Clamp->WMT = GS_CMODE_CLAMP;
	}
	else if(Preset == GS_CMODE_REGION_CLAMP)
	{
		gsGlobal->Clamp->WMS = GS_CMODE_REGION_CLAMP;
		gsGlobal->Clamp->WMT = GS_CMODE_REGION_CLAMP;
	}
	else if(Preset == GS_CMODE_REGION_REPEAT)
	{
		gsGlobal->Clamp->WMS = GS_CMODE_REGION_REPEAT;
		gsGlobal->Clamp->WMT = GS_CMODE_REGION_REPEAT;
	}

	p_data = p_store = gsKit_heap_alloc(gsGlobal, 1, 16, GIF_AD);

	*p_data++ = GIF_TAG_AD(1);
	*p_data++ = GIF_AD;

	*p_data++ = GS_SETREG_CLAMP(gsGlobal->Clamp->WMS, gsGlobal->Clamp->WMT,
				gsGlobal->Clamp->MINU, gsGlobal->Clamp->MAXU,
				gsGlobal->Clamp->MINV, gsGlobal->Clamp->MAXV);

	*p_data++ = GS_CLAMP_1+gsGlobal->PrimContext;
}
#endif

#if F_gsKit_set_primalpha
void gsKit_set_primalpha(GSGLOBAL *gsGlobal, u64 AlphaMode, u8 PerPixel)
{
	u64 *p_data;
	u64 *p_store;

	gsGlobal->PrimAlpha = AlphaMode;
	gsGlobal->PABE = PerPixel;

	p_data = p_store = gsKit_heap_alloc(gsGlobal, 2, 32, GIF_AD);

	*p_data++ = GIF_TAG_AD(2);
	*p_data++ = GIF_AD;

	*p_data++ = gsGlobal->PABE;
	*p_data++ = GS_PABE;

	*p_data++ = gsGlobal->PrimAlpha;
	*p_data++ = GS_ALPHA_1+gsGlobal->PrimContext;
}
#endif

#if F_gsKit_set_texfilter
void gsKit_set_texfilter(GSGLOBAL *gsGlobal, u8 FilterMode)
{
	u64 *p_data;
	u64 *p_store;

	p_data = p_store = gsKit_heap_alloc(gsGlobal, 1, 16, GIF_AD);

	*p_data++ = GIF_TAG_AD(1);
	*p_data++ = GIF_AD;

	*p_data++ = GS_SETREG_TEX1(0, 0, FilterMode, FilterMode, 0, 0, 0);
	*p_data++ = GS_TEX1_1+gsGlobal->PrimContext;
}
#endif

#if F_gsKit_set_texclut
void gsKit_set_texclut(GSGLOBAL *gsGlobal, gs_texclut texClut)
{
	u64 *p_data;
	u64 *p_store;

	p_data = p_store = gsKit_heap_alloc(gsGlobal, 1, 16, GIF_AD);

	*p_data++ = GIF_TAG_AD(1);
	*p_data++ = GIF_AD;

	memcpy(p_data, &texClut, sizeof(gs_texclut));
	p_data += 2; // Advance 2 u64, which is 16 bytes the gs_texclut struct size
}
#endif

#if F_gsKit_set_dither_matrix
void gsKit_set_dither_matrix(GSGLOBAL *gsGlobal)
{
    u64 *p_data;
    u64 *p_store;

    p_data = p_store = gsKit_heap_alloc(gsGlobal, 1 ,16, GIF_AD);

    *p_data++ = GIF_TAG_AD(1);
    *p_data++ = GIF_AD;

    *p_data++ = GS_SETREG_DIMX( gsGlobal->DitherMatrix[0],  gsGlobal->DitherMatrix[1],  gsGlobal->DitherMatrix[2],  gsGlobal->DitherMatrix[3],
                                gsGlobal->DitherMatrix[4],  gsGlobal->DitherMatrix[5],  gsGlobal->DitherMatrix[6],  gsGlobal->DitherMatrix[7],
                                gsGlobal->DitherMatrix[8],  gsGlobal->DitherMatrix[9], gsGlobal->DitherMatrix[10], gsGlobal->DitherMatrix[11],
                                gsGlobal->DitherMatrix[12], gsGlobal->DitherMatrix[13], gsGlobal->DitherMatrix[14], gsGlobal->DitherMatrix[15]);
    *p_data++ = GS_DIMX;
}
#endif

#if F_gsKit_set_dither
void gsKit_set_dither(GSGLOBAL *gsGlobal)
{
    u64 *p_data;
    u64 *p_store;

    p_data = p_store = gsKit_heap_alloc(gsGlobal, 1, 16, GIF_AD);

    *p_data++ = GIF_TAG_AD(1);
    *p_data++ = GIF_AD;

    if(gsGlobal->Dithering == GS_SETTING_ON)
        *p_data++ = 1;
    else
        *p_data++ = 0;

    *p_data++ = GS_DTHE;
}
#endif

#if F_gsKit_set_drawfield
// taken from libgraph
void gsKit_set_drawfield(GSGLOBAL *gsGlobal, u8 field)
{
    u64 *p_data;
    u64 *p_store;

    p_data = p_store = gsKit_heap_alloc(gsGlobal, 1 ,16, GIF_AD);

    *p_data++ = GIF_TAG_AD(1);
    *p_data++ = GIF_AD;

    if (field == GS_FIELD_NORMAL) { //Draw both
        *p_data++ = 0;;
    }
    else if (field == GS_FIELD_ODD) { // Draw only odd
        *p_data++ = 2;
    }
    else if (field == GS_FIELD_EVEN) { // Draw only even
        *p_data++ = 3;
    }

    *p_data++ = GS_SCANMSK;
}
/*
// taken from libgraph
void gsKit_set_drawfield(GSGLOBAL *gsGlobal, u8 field)
{
    u64 *p_data;
    u64 *p_store;

    (u32)p_data = (u32)p_store = gsGlobal->dma_misc;

    *p_data++ = GIF_TAG( 1, 1, 0, 0, GSKIT_GIF_FLG_PACKED, 1);
    *p_data++ = GIF_AD;

    if (field == GS_FIELD_NORMAL) { //Draw both
        *p_data++ = GIF_TAG_SCANMSK(0);
    }
    else if (field == GS_FIELD_ODD) { // Draw only odd
        *p_data++ = GIF_TAG_SCANMSK(2);
    }
    else if (field == GS_FIELD_EVEN) { // Draw only even
        *p_data++ = GIF_TAG_SCANMSK(3);
    }

    *p_data++ = GS_SCANMSK;

    dmaKit_wait_fast();
	dmaKit_send_ucab(DMA_CHANNEL_GIF, p_store, 2);
}
*/
#endif

#if F_gsKit_set_finish
GSQUEUE gsKit_set_finish(GSGLOBAL *gsGlobal)
{
	u64 *p_data;
	u64 *p_store;

	GSQUEUE oldQueue = *gsGlobal->CurQueue;

	p_data = p_store = gsKit_heap_alloc(gsGlobal, 1, 16, GIF_AD);

	*p_data++ = GIF_TAG_AD(1);
	*p_data++ = GIF_AD;

	*p_data++ = 0;
	*p_data++ = GS_FINISH;

	return oldQueue;
}
#endif

#if F_gsKit_queue_exec_real
void gsKit_queue_exec_real(GSGLOBAL *gsGlobal, GSQUEUE *Queue)
{
	GSQUEUE oldQueue;

	if(Queue->tag_size == 0)
		return;

	// This superstrange oldQueue crap is because Persistent drawbuffers need to be "backed up"
	// or else they will balloon in size due to appending the finish token.
	// So we back up the current *state* (NOT DATA) of them here and restore it afterward.
	oldQueue = gsKit_set_finish(gsGlobal);

	*(u64 *)Queue->dma_tag = DMA_TAG(Queue->tag_size, 0, DMA_END, 0, 0, 0);

	if(Queue->last_type != GIF_AD)
	{
		*(u64 *)Queue->last_tag = ((u64)Queue->same_obj | *(u64 *)Queue->last_tag);
	}

	if(!gsGlobal->FirstFrame)
		gsKit_finish();

	GS_SETREG_CSR_FINISH(1);

	dmaKit_wait_fast();

	dmaKit_send_chain_ucab(DMA_CHANNEL_GIF, Queue->pool[Queue->dbuf]);

	if(Queue->mode != GS_PERSISTENT)
	{
		Queue->dbuf  ^= 1;
		Queue->dma_tag = Queue->pool[Queue->dbuf];
		Queue->pool_cur = Queue->dma_tag + 16;
		Queue->last_type = GIF_RESERVED;
		Queue->last_tag = Queue->pool_cur;
		Queue->tag_size = 0;
	}
	else
	{
		*Queue = oldQueue;
		dmaKit_wait_fast();
	}
}
#endif

#if F_gsKit_queue_reset
void gsKit_queue_reset(GSQUEUE *Queue)
{
		if(Queue->mode == GS_ONESHOT)
		{
			Queue->dbuf  ^= 1;
		}

		Queue->dma_tag = Queue->pool[Queue->dbuf];
		Queue->pool_cur = Queue->dma_tag + 16;
		Queue->last_type = GIF_RESERVED;
		Queue->last_tag = Queue->pool_cur;
		Queue->tag_size = 0;
}
#endif

#if F_gsKit_queue_exec
void gsKit_queue_exec(GSGLOBAL *gsGlobal)
{
	GSQUEUE *CurQueue = gsGlobal->CurQueue;
	if(gsGlobal->DrawOrder == GS_PER_OS)
	{
		gsGlobal->CurQueue = gsGlobal->Per_Queue;
		gsKit_queue_exec_real(gsGlobal, gsGlobal->Per_Queue);
		gsGlobal->CurQueue = gsGlobal->Os_Queue;
		gsKit_queue_exec_real(gsGlobal, gsGlobal->Os_Queue);
	}
	else
	{
		gsGlobal->CurQueue = gsGlobal->Os_Queue;
		gsKit_queue_exec_real(gsGlobal, gsGlobal->Os_Queue);
		gsGlobal->CurQueue = gsGlobal->Per_Queue;
		gsKit_queue_exec_real(gsGlobal, gsGlobal->Per_Queue);
	}
	gsGlobal->CurQueue = CurQueue;
	gsGlobal->FirstFrame = GS_SETTING_OFF;
}
#endif

#if F_gsKit_alloc_ucab
void *gsKit_alloc_ucab(int size)
{
	// Allocate aligned memory
	u8 *p = memalign(64, size);

	// Flush the data cache
	SyncDCache(p, p+size-1);

	// Convert normal to UCAB memory
	p = (u8*)((u32)p | 0x30000000);

	// Return memory to UCAB area
	return p;
}
#endif

#if F_gsKit_free_ucab
void gsKit_free_ucab(void *p)
{
	// Convert UCAB to normal memory
	p = (u64 *)((u32)p & ~0x30000000);

	// Free normal memory
	free(p);
}
#endif

#if F_gsKit_queue_init
void gsKit_queue_init(GSGLOBAL *gsGlobal, GSQUEUE *Queue, u8 mode, int size)
{
	// Init pool 0
	Queue->pool[0]		= gsKit_alloc_ucab(size);
	Queue->pool_max[0]	= (u64 *)((u32)Queue->pool[0] + size);

	if (mode == GS_ONESHOT)
	{
		// Init pool 1
		Queue->pool[1]		= gsKit_alloc_ucab(size);
		Queue->pool_max[1]	= (u64 *)((u32)Queue->pool[1] + size);
	}

	Queue->dma_tag		= Queue->pool[0];
	Queue->pool_cur		= (u64 *)((u32)Queue->pool[0] + 16);
	Queue->dbuf			= 0;
	Queue->tag_size		= 0;
	Queue->last_tag		= Queue->pool_cur;
	Queue->last_type	= GIF_RESERVED;
	Queue->mode			= mode;
}
#endif

#if F_gsKit_queue_free
void gsKit_queue_free(GSGLOBAL *gsGlobal, GSQUEUE *Queue)
{
	if (Queue == NULL)
		return;

	if (Queue->pool[0] != NULL)
	{
		gsKit_free_ucab(Queue->pool[0]);
		Queue->pool[0] = NULL;
	}

	if (Queue->pool[1] != NULL)
	{
		gsKit_free_ucab(Queue->pool[1]);
		Queue->pool[1] = NULL;
	}
}
#endif

#if F_gsKit_queue_set
void gsKit_queue_set(GSGLOBAL *gsGlobal, GSQUEUE *Queue)
{
	gsGlobal->CurQueue = Queue;
}
#endif

#if F_gsKit_mode_switch
void gsKit_mode_switch(GSGLOBAL *gsGlobal, u8 mode)
{
	if(mode == GS_PERSISTENT)
	{
		gsKit_queue_set(gsGlobal, gsGlobal->Per_Queue);
	}
	else
	{
		gsKit_queue_set(gsGlobal, gsGlobal->Os_Queue);
	}
}
#endif
