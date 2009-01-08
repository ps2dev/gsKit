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

#include <stdio.h>
#include <kernel.h>

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
		#ifdef GSKIT_DEBUG
		printf("CurrentPointer After:\t0x%08X\n", gsGlobal->CurrentPointer);
		#endif
		return CurrentPointer;
	}
}

void gsKit_vram_clear(GSGLOBAL *gsGlobal)
{
    gsGlobal->CurrentPointer = gsGlobal->TexturePointer;
}

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
			gsGlobal->PrimContext ^= 1;
		}

	}

	gsKit_setactive(gsGlobal);

}

void gsKit_setactive(GSGLOBAL *gsGlobal)
{
	u64 *p_data;
	u64 *p_store;

	(u32)p_data = (u32)p_store = gsGlobal->dma_misc;

	*p_data++ = GIF_TAG( 4, 1, 0, 0, 0, 1 );
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

void gsKit_finish(void)
{
	while(!(GS_CSR_FINISH));
}

void gsKit_lock_buffer(GSGLOBAL *gsGlobal)
{
    gsGlobal->LockBuffer = GS_SETTING_ON;
}

void gsKit_unlock_buffer(GSGLOBAL *gsGlobal)
{
    gsGlobal->LockBuffer = GS_SETTING_OFF;
}

int gsKit_lock_status(GSGLOBAL *gsGlobal)
{
    return gsGlobal->LockBuffer;
}

void gsKit_display_buffer(GSGLOBAL *gsGlobal)
{
    GS_SET_DISPFB2( gsGlobal->ScreenBuffer[gsGlobal->ActiveBuffer & 1] / 8192,
            gsGlobal->Width / 64, gsGlobal->PSM, 0, 0 );
}

void gsKit_switch_context(GSGLOBAL *gsGlobal)
{
    gsGlobal->ActiveBuffer ^= 1;
    gsGlobal->PrimContext ^= 1;

    gsKit_setactive(gsGlobal);
}

void gsKit_vsync_wait(void)
{
	*GS_CSR = *GS_CSR & 8;
	while(!(*GS_CSR & 8));
}

void gsKit_vsync_nowait(void)
{
    *GS_CSR = *GS_CSR & 8;
}

void gsKit_get_field(GSGLOBAL *gsGlobal)
{
    gsGlobal->EvenOrOdd=((GSREG*)GS_CSR)->FIELD;
}

void gsKit_hsync_wait(void)
{
    *GS_CSR = *GS_CSR & 4;
    while(!(*GS_CSR & 4));
}

/*
enum
{
   kINTC_GS,
   kINTC_SBUS,
   kINTC_VBLANK_START,
   kINTC_VBLANK_END,
   kINTC_VIF0,
   kINTC_VIF1,
   kINTC_VU0,
   kINTC_VU1,
   kINTC_IPU,
   kINTC_TIMER0,
   kINTC_TIMER1
};
*/
int gsKit_add_vsync_handler(int (*vsync_callback)())
{
    int callback_id;

    DIntr();

    callback_id = AddIntcHandler(2, vsync_callback, 0);

    EnableIntc(2);

    EIntr();

    return callback_id;
}

void gsKit_remove_vsync_handler(int callback_id)
{
    DIntr();

    DisableIntc(2);

    RemoveIntcHandler(2, callback_id);

    EIntr();
}

void gsKit_clear(GSGLOBAL *gsGlobal, u64 color)
{
	u8 PrevZState = gsGlobal->Test->ZTST;
	gsKit_set_test(gsGlobal, GS_ZTEST_OFF);
	u8 strips = gsGlobal->Width / 64;
	u8 remain = gsGlobal->Width % 64;
	u32 pos = 0;

	strips++;
	while(strips-- > 0)
	{
		gsKit_prim_sprite(gsGlobal, pos, 0, pos + 64, gsGlobal->Height, 0, color);
		pos += 64;
	}
	if(remain > 0)
	{
		gsKit_prim_sprite(gsGlobal, pos, 0, remain + pos, gsGlobal->Height, 0, color);
	}

	gsGlobal->Test->ZTST = PrevZState;
	gsKit_set_test(gsGlobal, 0);
}

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

    *p_data++ = GIF_TAG( 1, 1, 0, 0, 0, 1);
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

void gsKit_queue_exec_real(GSGLOBAL *gsGlobal, GSQUEUE *Queue)
{
	if(Queue->tag_size == 0)
		return;

	// This superstrange oldQueue crap is because Persistent drawbuffers need to be "backed up"
	// or else they will balloon in size due to appending the finish token.
	// So we back up the current *state* (NOT DATA) of them here and restore it afterward.
	GSQUEUE oldQueue = gsKit_set_finish(gsGlobal);

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

void gsKit_queue_reset(GSQUEUE *Queue)
{
		if(Queue->mode == GS_ONESHOT)
		{
			Queue->dbuf  ^= 1;
			Queue->dma_tag = Queue->pool[Queue->dbuf];
		}

		Queue->pool_cur = Queue->dma_tag + 16;
		Queue->last_type = GIF_RESERVED;
		Queue->last_tag = Queue->pool_cur;
		Queue->tag_size = 0;
}

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

void gsKit_mode_switch(GSGLOBAL *gsGlobal, u8 mode)
{
	if(mode == GS_PERSISTENT)
	{
		gsGlobal->CurQueue = gsGlobal->Per_Queue;
	}
	else
	{
		gsGlobal->CurQueue = gsGlobal->Os_Queue;
	}
}
