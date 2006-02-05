//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsCore.c - C/Assembler implimentation of all GS instructions.
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

u32 gsKit_vram_alloc(GSGLOBAL *gsGlobal, u32 size, u8 type)
{
	u32 CurrentPointer = gsGlobal->CurrentPointer;
	#ifdef DEBUG
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
		#ifdef DEBUG
		printf("CurrentPointer After:\t0x%08X\n", gsGlobal->CurrentPointer);
		#endif
		return CurrentPointer;
	}
}

void gsKit_vram_free(GSGLOBAL *gsGlobal, GSTEXTURE *Texture)
{
	
}

void gsKit_sync_flip(GSGLOBAL *gsGlobal)
{
	gsKit_vsync();
  
	GS_SET_DISPFB2( gsGlobal->ScreenBuffer[gsGlobal->ActiveBuffer & 1] / 8192,
		gsGlobal->Width / 64, gsGlobal->PSM, 0, 0 );
  
	gsGlobal->ActiveBuffer ^= 1;
	gsGlobal->PrimContext ^= 1;

	if(gsGlobal->DoSubOffset == GS_SETTING_ON)
		gsGlobal->EvenOrOdd=((GSREG*)GS_CSR)->FIELD;

	gsKit_setactive(gsGlobal);
}

void gsKit_setactive(GSGLOBAL *gsGlobal)
{
	u64 *p_data;
	u64 *p_store;

	p_data = p_store = dmaKit_spr_alloc( 5*16 );

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

	dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, 5 );
	dmaKit_wait_fast( DMA_CHANNEL_GIF );
}

void gsKit_vsync(void)
{
	*GS_CSR = *GS_CSR & 8;
	while(!(*GS_CSR & 8));
}

void gsKit_clear(GSGLOBAL *gsGlobal, u64 color)
{
	u8 PrevZState = gsGlobal->Test->ZTST;
	gsKit_set_test(gsGlobal, GS_ZTEST_OFF);
	u8 strips = gsGlobal->Width / 32;
	u8 remain = gsGlobal->Width % 32;
	u32 pos = 0;
	
	strips++;
	while(strips-- > 0)
	{
		gsKit_prim_sprite(gsGlobal, pos, 0, pos + 32, gsGlobal->Height, 0, color);
		pos += 32;
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
	if(gsGlobal->ZBuffering == GS_SETTING_OFF)
		return;

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

	p_data = p_store = gsKit_spr_alloc(gsGlobal, 1, 16);

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

	p_data = p_store = gsKit_spr_alloc(gsGlobal, 1, 16);

	*p_data++ = GS_SETREG_CLAMP(gsGlobal->Clamp->WMS, gsGlobal->Clamp->WMT, 
				gsGlobal->Clamp->MINU, gsGlobal->Clamp->MAXU, 
				gsGlobal->Clamp->MINV, gsGlobal->Clamp->MAXV);

	*p_data++ = GS_CLAMP_1+gsGlobal->PrimContext;

}

void gsKit_kick_spr(GSGLOBAL *gsGlobal, int size)
{
	        if(size > 1024)
        	        return;

                if((gsGlobal->CurQueue->size + size) > gsGlobal->CurQueue->maxsize)
                        gsKit_queue_exec_real(gsGlobal, gsGlobal->CurQueue);

                u32 offset = (u32)gsGlobal->CurQueue->spr_cur - 0x70000000;
                (u32)gsGlobal->CurQueue->spr_cur = 0x70000000;
                dmaKit_get_spr(DMA_CHANNEL_FROMSPR, (void *)0x70000000, gsGlobal->CurQueue->pool_cur, offset / 16);
                dmaKit_wait_fast(DMA_CHANNEL_FROMSPR);
                gsGlobal->CurQueue->pool_cur += offset;
}

void gsKit_queue_exec_real(GSGLOBAL *gsGlobal, GSQUEUE *Queue)
{
	u32 offset;

	if(Queue->size == 0)
		return;

        if((u32)Queue->spr_cur > 0x70000000)
        {
                offset = (u32)Queue->spr_cur - 0x70000000;
		dmaKit_get_spr(DMA_CHANNEL_FROMSPR, (void *)0x70000000, Queue->pool_cur, offset / 16);
		dmaKit_wait_fast(DMA_CHANNEL_FROMSPR);
		Queue->size += (offset / 16);
        }

        u64* p_store;
        u64* p_data;

	u32 numpackets = Queue->size / 1022;
	u32 remain = Queue->size % 1022;

        (u32)p_store = 0x70000000; // First 16k (RenderChain -> DMA)

	Queue->pool_cur = Queue->pool;

	p_data = p_store;

	*p_data++ = DMA_TAG(1023, 0, DMA_END, 0, 0, 0);
	*p_data++ = 0;

	*p_data++ = GIF_TAG(1022, 0, 0, 0, 0, 1);
	*p_data++ = GIF_AD;

	while(numpackets > 0)
	{
		dmaKit_get_spr(DMA_CHANNEL_TOSPR, p_data, Queue->pool_cur, 1022);
                dmaKit_wait_fast(DMA_CHANNEL_TOSPR);

		(u32)Queue->pool_cur += 16352;
		
		dmaKit_send_chain_spr( DMA_CHANNEL_GIF, 0, p_store);
                dmaKit_wait_fast(DMA_CHANNEL_GIF);
		numpackets--;
	}

	if(remain > 0)
 	{
		p_data = p_store;

	        *p_data++ = DMA_TAG(remain + 1, 0, DMA_END, 0, 0, 0);
	        *p_data++ = 0;

	        *p_data++ = GIF_TAG(remain, 0, 0, 0, 0, 1);
	        *p_data++ = GIF_AD;

		dmaKit_get_spr(DMA_CHANNEL_TOSPR, p_data, Queue->pool_cur, remain);
                dmaKit_wait_fast(DMA_CHANNEL_TOSPR);
		
		dmaKit_send_chain_spr( DMA_CHANNEL_GIF, 0, p_store);
                dmaKit_wait_fast(DMA_CHANNEL_GIF);
	}
	
	Queue->pool_cur = Queue->pool;
	(u32)Queue->spr_cur = 0x70000000;
	
	if(Queue->mode == GS_ONESHOT)
		Queue->size = 0;

}

void gsKit_queue_exec(GSGLOBAL *gsGlobal)
{
	GSQUEUE *CurQueue = gsGlobal->CurQueue;
	if(gsGlobal->DrawOrder == GS_PER_OS)
	{
		gsKit_mode_switch(gsGlobal, GS_PERSISTENT);
		gsKit_queue_exec_real(gsGlobal, gsGlobal->Per_Queue);
		gsKit_mode_switch(gsGlobal, GS_ONESHOT);
		gsKit_queue_exec_real(gsGlobal, gsGlobal->Os_Queue);
	}
	else
	{
		gsKit_mode_switch(gsGlobal, GS_ONESHOT);
		gsKit_queue_exec_real(gsGlobal, gsGlobal->Os_Queue);
		gsKit_mode_switch(gsGlobal, GS_PERSISTENT);
		gsKit_queue_exec_real(gsGlobal, gsGlobal->Per_Queue);
	}
	gsGlobal->CurQueue = CurQueue;
}

void gsKit_mode_switch(GSGLOBAL *gsGlobal, u8 mode)
{
	u32 offset;
	if(mode == GS_PERSISTENT)
	{
	        if((u32)gsGlobal->Os_Queue->spr_cur > 0x70000000)
	        {
	                offset = (u32)gsGlobal->Os_Queue->spr_cur - 0x70000000;
	                dmaKit_get_spr(DMA_CHANNEL_FROMSPR, (void *)0x70000000, gsGlobal->Os_Queue->pool_cur, offset / 16);
	                dmaKit_wait_fast(DMA_CHANNEL_FROMSPR);
	                gsGlobal->Os_Queue->size += (offset / 16);
			(u32)gsGlobal->Os_Queue->spr_cur = 0x70000000;
	        }
		gsGlobal->CurQueue = gsGlobal->Per_Queue;
	}
	else
	{
                if((u32)gsGlobal->Per_Queue->spr_cur > 0x70000000)
                {
                        offset = (u32)gsGlobal->Per_Queue->spr_cur - 0x70000000;
	                dmaKit_get_spr(DMA_CHANNEL_FROMSPR, (void *)0x70000000, gsGlobal->Per_Queue->pool_cur, offset / 16);
                        dmaKit_wait_fast(DMA_CHANNEL_FROMSPR);
                        gsGlobal->Per_Queue->size += (offset / 16);
                        (u32)gsGlobal->Per_Queue->spr_cur = 0x70000000;
                }
		gsGlobal->CurQueue = gsGlobal->Os_Queue;
	}
}
