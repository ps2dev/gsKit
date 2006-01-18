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

	p_data = p_store = dmaKit_spr_alloc(2*16);

        if(gsGlobal->DrawMode == GS_IMMEDIATE)
	{
		*p_data++ = GIF_TAG( 1, 1, 0, 0, 0, 1 );
		*p_data++ = GIF_AD;
	}

	*p_data++ = GS_SETREG_TEST( gsGlobal->Test->ATE, gsGlobal->Test->ATST,
				    gsGlobal->Test->AREF, gsGlobal->Test->AFAIL, 
				    gsGlobal->Test->DATE, gsGlobal->Test->DATM, 
				    gsGlobal->Test->ZTE, gsGlobal->Test->ZTST );

	*p_data++ = GS_TEST_1+gsGlobal->PrimContext;

        if(gsGlobal->DrawMode == GS_IMMEDIATE)
	{
                dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, 2 );
		dmaKit_wait_fast( DMA_CHANNEL_GIF );
	}
        else
                gsKit_queue_add( gsGlobal, DMA_CHANNEL_GIF, p_store, 1 );
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

	p_data = p_store = dmaKit_spr_alloc(2*16);	

        if(gsGlobal->DrawMode == GS_IMMEDIATE)
	{
	        *p_data++ = GIF_TAG( 1, 1, 0, 0, 0, 1 );
	        *p_data++ = GIF_AD;
	}

	*p_data++ = GS_SETREG_CLAMP(gsGlobal->Clamp->WMS, gsGlobal->Clamp->WMT, 
				gsGlobal->Clamp->MINU, gsGlobal->Clamp->MAXU, 
				gsGlobal->Clamp->MINV, gsGlobal->Clamp->MAXV);

	*p_data++ = GS_CLAMP_1+gsGlobal->PrimContext;

        if(gsGlobal->DrawMode == GS_IMMEDIATE)
	{
                dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, 2 );
                dmaKit_wait_fast( DMA_CHANNEL_GIF );
	}
        else
                gsKit_queue_add( gsGlobal, DMA_CHANNEL_GIF, p_store, 1 );
}

void gsKit_queue_add(GSGLOBAL *gsGlobal, u8 channel, void *data, u32 size)
{
	if(gsGlobal->DrawMode == GS_ONESHOT)
	{
                if((gsGlobal->Queue->os_size + size) > GS_RENDER_QUEUE_OS_POOLSIZE)
                {
                        printf("too much for renderbuffer, flushing.\n");
                        // This is to handle worst case that we overflow the queue... slight performance hit but only for this prim.
                        u32 *tmpdata = malloc(size * 16);
                        dmaKit_get_spr(DMA_CHANNEL_FROMSPR, data, tmpdata,  size);
                        gsKit_queue_exec_os(gsGlobal);
                        memcpy(gsGlobal->Queue->os_pool_cur, tmpdata, size * 16);
                }
                else
                {
                        dmaKit_get_spr(DMA_CHANNEL_FROMSPR, data, gsGlobal->Queue->os_pool_cur, size);
                }

                gsGlobal->Queue->os_size += size;
                (u32)gsGlobal->Queue->os_pool_cur += (size * 16);
	}
	else
	{
                GSELEMENT* Element = &gsGlobal->Queue->Per_Elements[gsGlobal->Queue->per_lastElement];

                Element->size = size;

                dmaKit_get_spr(DMA_CHANNEL_FROMSPR, data, Element->data, size);

                gsGlobal->Queue->per_size += size;
                gsGlobal->Queue->per_numElements++;
                gsGlobal->Queue->per_lastElement++;

                if(gsGlobal->Queue->per_numElements >= (GS_RENDER_QUEUE_PER_MAX - 1))
                {
                        gsKit_queue_exec_per(gsGlobal);
                        return;
                }
	}
	
//	GSKIT_PREFETCH(&gsGlobal->Queue->Elements[gsGlobal->Queue->lastElement]);
}

void gsKit_queue_exec_per(GSGLOBAL *gsGlobal)
{
	if(gsGlobal->Queue->per_numElements == 0)
		return;

        u64* p_store[3];
        u64* p_data[2];
        u32 p_limit[3];
        GSELEMENT* Element;
        u8 batch = 0;
        u32 p_size = 0;

        s32 numpackets = gsGlobal->Queue->per_numElements;

        (u32)p_store[0] = 0x70000000; // First 4k (RenderChain -> DMA)
        (u32)p_store[1] = 0x70001000; // Second 4k (RenderChain -> DMA)
        (u32)p_store[2] = 0x70002000; // Last 8k (Element Array -> SPR)

        p_limit[0] = 0x70000FFF; // First Block's Limit
        p_limit[1] = 0x70001FFF; // Second Block's Limit
        p_limit[2] = 0x70003FFF; // Last Block's Limit
        (u32)Element = p_store[2];

        dmaKit_get_spr(DMA_CHANNEL_TOSPR, Element, &gsGlobal->Queue->Per_Elements[0], numpackets * 4);
        dmaKit_wait_fast(DMA_CHANNEL_TOSPR);

	FlushCache(0);

        while(numpackets > 0)
        {
                (u32)p_data[batch] = (u32)p_store[batch] + 64;

                while(numpackets-- > 0)
                {
			dmaKit_get_spr(DMA_CHANNEL_TOSPR, p_data[batch], Element->data, Element->size);
                        dmaKit_wait_fast(DMA_CHANNEL_TOSPR);

                        (u32)p_data[batch] += (Element->size * 16);

                        Element++;
			if(((u32)p_data[batch] + (Element->size * 16))  > (u32)p_limit[batch])
                        {
                                break;
                        }
                }

                p_size = ((u32)p_data[batch] - (u32)p_store[batch]) / 16;

                p_data[batch] = p_store[batch];

                *p_data[batch]++ = DMA_TAG(p_size - 1, 0, DMA_END, 0, 0, 0);
                *p_data[batch]++ = 0;

                *p_data[batch]++ = GIF_TAG(p_size - 2, 0, 0, 0, 0, 1);
                *p_data[batch]++ = GIF_AD;

                dmaKit_wait_fast(DMA_CHANNEL_GIF);
		dmaKit_send_chain_spr( DMA_CHANNEL_GIF, 0, p_store[batch]);
		batch ^= 1;
        }

	dmaKit_wait_fast(DMA_CHANNEL_GIF);

}

void gsKit_queue_exec_os(GSGLOBAL *gsGlobal)
{
	if(gsGlobal->Queue->os_size == 0)
		return;

        u64* p_store;
        u64* p_data;

	u32 numpackets = gsGlobal->Queue->os_size / 1022;
	u32 remain = gsGlobal->Queue->os_size % 1022;

        (u32)p_store = 0x70000000; // First 16k (RenderChain -> DMA)

	gsGlobal->Queue->os_pool_cur = gsGlobal->Queue->os_pool;

	p_data = p_store;

	*p_data++ = DMA_TAG(1023, 0, DMA_END, 0, 0, 0);
	*p_data++ = 0;

	*p_data++ = GIF_TAG(1022, 0, 0, 0, 0, 1);
	*p_data++ = GIF_AD;

	while(numpackets > 0)
	{
		dmaKit_get_spr(DMA_CHANNEL_TOSPR, p_data, gsGlobal->Queue->os_pool_cur, 1022);
                dmaKit_wait_fast(DMA_CHANNEL_TOSPR);

		(u32)gsGlobal->Queue->os_pool_cur += 16352;
		
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

		dmaKit_get_spr(DMA_CHANNEL_TOSPR, p_data, gsGlobal->Queue->os_pool_cur, remain);
                dmaKit_wait_fast(DMA_CHANNEL_TOSPR);
		
		dmaKit_send_chain_spr( DMA_CHANNEL_GIF, 0, p_store);
	}
	
	gsGlobal->Queue->os_pool_cur = gsGlobal->Queue->os_pool;
	gsGlobal->Queue->os_size = 0;
}

void gsKit_queue_exec(GSGLOBAL *gsGlobal)
{
	if(gsGlobal->DrawOrder == GS_PER_OS)
	{
		gsKit_queue_exec_per(gsGlobal);
		gsKit_queue_exec_os(gsGlobal);
	}
	else
	{
		gsKit_queue_exec_os(gsGlobal);
		gsKit_queue_exec_per(gsGlobal);
	}
}

