//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2017 - Rick "Maximus32" Gaiser <rgaiser@gmail.com>
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsHires.c - Multi pass high resolution rendering
//

#include <gsKit.h>
#include "gsInline.h"
#include <kernel.h>
#include <malloc.h>


/*
 *  -------------------------------------------------------------------------
 *  2 pass rendering
 *  Note: GS draw speed must be 60fps == 120 passes/sec
 *  -------------------------------------------------------------------------
 *
 *            Render Pass1                      Render Pass2
 *
 *  +------------------------------+  +------------------------------+
 *  |         Front-Buffer         |  |             CRTC             |
 *  +------------------------------+  +------------------------------+
 *  |             CRTC             |  |         Front-Buffer         |
 *  +------------------------------+  +------------------------------+
 *
 *  +------------------------------+
 *  |         Depth-Buffer         |
 *  +------------------------------+
 *  |           Textures           |
 *  +------------------------------+
 *
 *
 *  -------------------------------------------------------------------------
 *  3 pass rendering
 *  Note: GS draw speed must be 60fps == 180 passes/sec
 *  -------------------------------------------------------------------------
 *
 *            Render Pass1                      Render Pass2                      Render Pass3
 *
 *  +------------------------------+  +------------------------------+  +------------------------------+
 *  |         Front-Buffer         |  |             CRTC             |  |         Depth-Buffer         |
 *  +------------------------------+  +------------------------------+  +------------------------------+
 *  |         Depth-Buffer         |  |         Front-Buffer         |  |             CRTC             |
 *  +------------------------------+  +------------------------------+  +------------------------------+
 *  |             CRTC             |  |         Depth-Buffer         |  |         Front-Buffer         |
 *  +------------------------------+  +------------------------------+  +------------------------------+
 *
 *  +------------------------------+
 *  |           Textures           |
 *  +------------------------------+
 *
 *
 *  -------------------------------------------------------------------------
 *  4 pass rendering
 *  Note: GS draw speed must be 60fps == 240 passes/sec
 *  -------------------------------------------------------------------------
 *
 *            Render Pass1                      Render Pass2                      Render Pass3                      Render Pass4
 *
 *  +------------------------------+  +------------------------------+  +------------------------------+  +------------------------------+
 *  |         Front-Buffer         |  |             CRTC             |  |         Depth-Buffer         |  |         Depth-Buffer         |
 *  +------------------------------+  +------------------------------+  +------------------------------+  +------------------------------+
 *  |           Textures           |  |         Front-Buffer         |  |             CRTC             |  |           Textures           |
 *  +------------------------------+  +------------------------------+  +------------------------------+  +------------------------------+
 *  |         Depth-Buffer         |  |         Depth-Buffer         |  |         Front-Buffer         |  |             CRTC             |
 *  +------------------------------+  +------------------------------+  +------------------------------+  +------------------------------+
 *  |             CRTC             |  |           Textures           |  |           Textures           |  |         Front-Buffer         |
 *  +------------------------------+  +------------------------------+  +------------------------------+  +------------------------------+
 *
 *  +------------------------------+
 *  |           Textures           |
 *  +------------------------------+
 *
 */


//#define BG_DEBUG


typedef struct {
	u32 addr;

	u32 bufferline_begin;
	u32 bufferline_end;

	u32 scanline_begin;
	u32 scanline_end;
} SBuffer;

typedef struct {
	SBuffer * DrawBuffer;
	SBuffer * DepthBuffer;
	SBuffer * TextBuffer;
	SBuffer * CRTCBuffer;
	GSQUEUE Queue;
	GSQUEUE QueueIF; // Only for Interlaced Frame mode
} SPass;

extern void *_gp;
#define MAX_PASS_COUNT 4
#define STACK_SIZE (8 * 1024)

#if F_gsHiresIinternals
GSQUEUE __DrawQueue[2];
volatile u32 __iCurrentDrawQueue = 0;
GSTEXTURE * __pTexture = NULL;

u32 __iYStart;
s32 __hsync_callback_id = -1;
volatile u32 __vsync_count = 0;
volatile u32 __hsync_count = 0;

u32 __iPassCount;
u32 __iCurrentPass = 0;
u32 __iCurrentRenderField = 0;
SBuffer __buffer[MAX_PASS_COUNT];
SPass  __pass[MAX_PASS_COUNT];

s32 __sema_vsync_id = 0;
s32 __sema_hsync_id = 0;
s32 __sema_queue_id = 0;

u8 __ThreadStack[STACK_SIZE] __attribute__ ((aligned(16)));
int __ThreadId = -1;

#else

extern GSQUEUE __DrawQueue[];
extern volatile u32 __iCurrentDrawQueue;
extern GSTEXTURE * __pTexture;

extern u32 __iYStart;
extern s32 __hsync_callback_id;
extern volatile u32 __vsync_count;
extern volatile u32 __hsync_count;

extern u32 __iPassCount;
extern u32 __iCurrentPass;
extern u32 __iCurrentRenderField;
extern SBuffer __buffer[];
extern SPass  __pass[];

extern s32 __sema_vsync_id;
extern s32 __sema_hsync_id;
extern s32 __sema_queue_id;

extern u8 __ThreadStack[];
extern int __ThreadId;
#endif

#if F__gsKit_create_passes
static void gsKit_setpass(GSGLOBAL *gsGlobal, const SPass * __pass, int yoff)
{
	u64 *p_data;
	u64 *p_store;
	int qsize = 8;

	p_data = p_store = (u64*)gsKit_heap_alloc(gsGlobal, qsize, (qsize * 16), GIF_AD);

	*p_data++ = GIF_TAG_AD(qsize);
	*p_data++ = GIF_AD;

	// Context 1
	*p_data++ = GS_SETREG_SCISSOR_1(0, gsGlobal->Width - 1, 0, __pass->DrawBuffer->bufferline_end - __pass->DrawBuffer->bufferline_begin);
	*p_data++ = GS_SCISSOR_1;
	*p_data++ = GS_SETREG_FRAME_1(__pass->DrawBuffer->addr / 8192, gsGlobal->Width / 64, gsGlobal->PSM, 0);
	*p_data++ = GS_FRAME_1;
	*p_data++ = GS_SETREG_ZBUF_1(__pass->DepthBuffer->addr / 8192, gsGlobal->PSMZ, (gsGlobal->ZBuffering == GS_SETTING_ON) ? 0 : 1);
	*p_data++ = GS_ZBUF_1;
	*p_data++ = GS_SETREG_XYOFFSET_1(gsGlobal->OffsetX, yoff + gsGlobal->OffsetY + __pass->DrawBuffer->bufferline_begin*16);
	*p_data++ = GS_XYOFFSET_1;

	// Context 2
	*p_data++ = GS_SETREG_SCISSOR_1(0, gsGlobal->Width - 1, 0, __pass->DrawBuffer->bufferline_end - __pass->DrawBuffer->bufferline_begin);
	*p_data++ = GS_SCISSOR_2;
	*p_data++ = GS_SETREG_FRAME_1(__pass->DrawBuffer->addr / 8192, gsGlobal->Width / 64, gsGlobal->PSM, 0);
	*p_data++ = GS_FRAME_2;
	*p_data++ = GS_SETREG_ZBUF_1(__pass->DepthBuffer->addr / 8192, gsGlobal->PSMZ, (gsGlobal->ZBuffering == GS_SETTING_ON) ? 0 : 1);
	*p_data++ = GS_ZBUF_2;
	*p_data++ = GS_SETREG_XYOFFSET_1(gsGlobal->OffsetX, yoff + gsGlobal->OffsetY + __pass->DrawBuffer->bufferline_begin*16);
	*p_data++ = GS_XYOFFSET_2;
}

static void gsKit_setpass_background(GSGLOBAL *gsGlobal, const SPass * __pass, GSTEXTURE * tex)
{
	u16 width = gsGlobal->Width;
	u16 height = __pass->DrawBuffer->bufferline_end - __pass->DrawBuffer->bufferline_begin + 1;
	void * mem = &((u16*)tex->Mem)[__pass->DrawBuffer->bufferline_begin*width];

	// Transfer the texture directly into the drawbuffer
	gsKit_texture_send_inline(gsGlobal, mem, width, height, __pass->DrawBuffer->addr, tex->PSM, tex->TBW, GS_CLUT_NONE);
}

static void gsKit_create_pass(GSGLOBAL *gsGlobal, const SPass * __pass, GSQUEUE * Queue, int yoff)
{
	GSQUEUE * Queue_backup = gsGlobal->CurQueue;

	// Create new queue (if not already created)
	if (Queue->pool[0] == NULL)
		gsKit_queue_init(gsGlobal, Queue, GS_PERSISTENT, 8*1024);
	gsKit_queue_set(gsGlobal, Queue);
	gsKit_queue_reset(Queue);

	// Set the rendering context
	gsKit_setpass(gsGlobal, __pass, yoff);

	// Clear depth buffer (and background)
	if ((gsGlobal->ZBuffering == GS_SETTING_ON) || (__pTexture == NULL))
		gsKit_clear(gsGlobal, GS_SETREG_RGBAQ(0x00,0x00,0x00,0x00,0x00));

	// Fill with background image
	if (__pTexture != NULL)
		gsKit_setpass_background(gsGlobal, __pass, __pTexture);

	// Restore previous queue
	gsKit_queue_set(gsGlobal, Queue_backup);
}

void _gsKit_create_passes(GSGLOBAL *gsGlobal)
{
	u32 iPass;

	// Create draw queues to setup the rendering for each pass
	if ((gsGlobal->Interlace == GS_INTERLACED) && (gsGlobal->Field == GS_FRAME)) {
		for (iPass = 0; iPass < __iPassCount; iPass++) {
			// ODD lines
			gsKit_create_pass(gsGlobal, &__pass[iPass], &__pass[iPass].Queue, 0);
			// EVEN lines, half a pixel DOWN
			gsKit_create_pass(gsGlobal, &__pass[iPass], &__pass[iPass].QueueIF, 8);
		}
	}
	else {
		for (iPass = 0; iPass < __iPassCount; iPass++) {
			gsKit_create_pass(gsGlobal, &__pass[iPass], &__pass[iPass].Queue, 0);
		}
	}
}
#else
void _gsKit_create_passes(GSGLOBAL *gsGlobal);
#endif

#if F_gsKit_hires_sync
void gsKit_hires_sync(GSGLOBAL *gsGlobal)
{
	// Make sure the sema count == 0
	while (PollSema(__sema_vsync_id) >= 0)
		;

	// Wait for vsync
	WaitSema(__sema_vsync_id);
}
#endif

#if F_gsKit_hires_flip
void gsKit_hires_flip(GSGLOBAL *gsGlobal)
{
	u32 iY;

	// HACK: The start of the first displayed line
	// this is needed by the hsync interrupt but must be
	// updated every time StartYOffset changes
	iY = gsGlobal->StartY + gsGlobal->StartYOffset + 4;
	if (gsGlobal->Interlace == GS_INTERLACED)
		iY /= 2;
	__iYStart = iY;

	// Toggle draw queue
	WaitSema(__sema_queue_id);
	dmaKit_wait_fast();
	__iCurrentDrawQueue = (__iCurrentDrawQueue == 0) ? 1 : 0;
	SignalSema(__sema_queue_id);

	gsKit_queue_reset(&__DrawQueue[__iCurrentDrawQueue]);
	gsKit_queue_set(gsGlobal, &__DrawQueue[__iCurrentDrawQueue]);
}
#endif

#if F_gsKit_hires_prepare_bg
static void gsKit_texture_to_interlaced_frame(GSTEXTURE * tex)
{
	int y;
	int bpp;
	int size;
	u16 *tex_new;
	u16 *tex_old;

	switch (tex->PSM)
	{
		case GS_PSM_CT16S: bpp = 2; break;
		case GS_PSM_CT24:  bpp = 3; break;
		default: return; // not supported
	};

	size = tex->Width * tex->Height * bpp;
	tex_new = (u16*)memalign(128, size);
	tex_old = (u16*)tex->Mem;

	for(y=0; y < tex->Height; y+=2) {
		// Copy 0,2,4... to 0,1,2...
		memcpy(&tex_new[(y/2)*tex->Width], &tex_old[y*tex->Width], tex->Width * bpp);
		// Copy 1,3,5... to (height/2) + 0,1,2...
		memcpy(&tex_new[((y+tex->Height)/2)*tex->Width], &tex_old[(y+1)*tex->Width], tex->Width * bpp);
	}

	free(tex->Mem);
	tex->Mem = (void*)tex_new;
}

void gsKit_hires_prepare_bg(GSGLOBAL *gsGlobal, GSTEXTURE * tex)
{
	// Convert to same color format
	if (tex->PSM != gsGlobal->PSM) {
		if (gsGlobal->PSM == GS_PSM_CT16S)
			gsKit_texture_to_psm16(tex);
		else
			return; // not supported
	}

	// Convert to interlaced (if needed)
	if ((gsGlobal->Interlace == GS_INTERLACED) && (gsGlobal->Field == GS_FRAME))
		gsKit_texture_to_interlaced_frame(tex);
}
#endif

#if F_gsKit_hires_set_bg
int gsKit_hires_set_bg(GSGLOBAL *gsGlobal, GSTEXTURE * tex)
{
	// Validate width
	if (tex->Width != gsGlobal->Width)
		return -1;

	// Validate height
	if ((gsGlobal->Interlace == GS_INTERLACED) && (gsGlobal->Field == GS_FRAME)) {
		if (tex->Height != gsGlobal->Height*2)
			return -1;
	}
	else {
		if (tex->Height != gsGlobal->Height)
			return -1;
	}

	// Validate PSM
	if (tex->PSM != gsGlobal->PSM)
		return -1;

	// Accept background texture
	__pTexture = tex;
	// (Re-)create pass queues
	gsKit_hires_sync(gsGlobal);
	WaitSema(__sema_queue_id);
	dmaKit_wait_fast();
	_gsKit_create_passes(gsGlobal);
	SignalSema(__sema_queue_id);

	return 0;
}
#endif

#if F_gsKit_hires_init_screen
static int hsync_callback()
{
	u32 iPass;

	if (*GS_CSR & (1<<2)) {
		*GS_CSR = (1<<2);
		__hsync_count++;
	}

	if (*GS_CSR & (1<<3)) {
		*GS_CSR = (1<<3);
		__vsync_count++;
		__hsync_count=0;
	}

	for (iPass = 0; iPass < __iPassCount; iPass++) {
		if (__hsync_count == (__iYStart + __pass[iPass].CRTCBuffer->scanline_begin)) {
			if (iPass == 0) {
				// We start rendering the first pass when the CRTC starts
				// displaying the last pass of the previous field.
				// So invert to get the current render field.
				__iCurrentRenderField = (((GSREG*)GS_CSR)->FIELD == 1) ? 0 : 1;
			}

			__iCurrentPass = iPass;
			iSignalSema(__sema_hsync_id);

#ifdef BG_DEBUG
			GS_SET_BGCOLOR((iPass+1)*63, (iPass+1)*63, (iPass+1)*63);
#endif

			break;
		}
	}

	ExitHandler();
	return 0;
}

static void gsKit_queue_send(GSGLOBAL *gsGlobal, GSQUEUE *Queue)
{
	if(Queue->tag_size == 0)
		return;

	*(u64 *)Queue->dma_tag = DMA_TAG(Queue->tag_size, 0, DMA_END, 0, 0, 0);

	if(Queue->last_type != GIF_AD)
	{
		*(u64 *)Queue->last_tag = ((u64)Queue->same_obj | *(u64 *)Queue->last_tag);
	}

	dmaKit_wait_fast();
	dmaKit_send_chain_ucab(DMA_CHANNEL_GIF, Queue->pool[Queue->dbuf]);
}

static void dma_to_gs_thread(void * data)
{
	GSGLOBAL *gsGlobal = (GSGLOBAL *)data;
	u32 iQueue;

	while(1)
	{
		// Make sure the sema count == 0
		while (PollSema(__sema_hsync_id) >= 0)
			;

		// Wait for CRTC
		WaitSema(__sema_hsync_id);

		WaitSema(__sema_queue_id);

#ifdef BG_DEBUG
		GS_SET_BGCOLOR(128,0,0);
#endif

		// Send current pass queue
		if ((gsGlobal->Interlace == GS_INTERLACED) && (gsGlobal->Field == GS_FRAME)) {
			if (__iCurrentRenderField == 1)
				gsKit_queue_send(gsGlobal, &__pass[__iCurrentPass].Queue); // Odd
			else
				gsKit_queue_send(gsGlobal, &__pass[__iCurrentPass].QueueIF); // Even
		}
		else {
			gsKit_queue_send(gsGlobal, &__pass[__iCurrentPass].Queue);
		}

		// Send current frame queue
		iQueue = (__iCurrentDrawQueue == 0) ? 1 : 0;
		gsKit_queue_send(gsGlobal, &__DrawQueue[iQueue]);

#ifdef BG_DEBUG
		GS_SET_BGCOLOR(0,128,0);
#endif

		SignalSema(__sema_queue_id);

		// If this was the last pass, send vsync
		if (__iCurrentPass == (__iPassCount-1))
			SignalSema(__sema_vsync_id);
	}
}

void gsKit_hires_init_screen(GSGLOBAL *gsGlobal, int passCount)
{
	u8 ZBuffering_backup;
	ee_sema_t sema;
	u32 iPass;
	u32 iBufferline;
	u32 iPassHeight;
	u32 iBpp;
	u32 iHeightAlign;
	ee_thread_t thread;

	// Limit __iPassCount between 2 and 4:
	// 2 Passes: Front buffer split in 2, used as front and back buffer. Separate depth buffer (half the size) needed.
	// 3 Passes: Front buffer split in 3, used as front, back and depth buffer.
	// 4 Passes: Front buffer split in 4, used as front, back and depth buffer + some temporary texture space (currently not used).
	__iPassCount = passCount;
	__iPassCount = __iPassCount < 2 ? 2 : __iPassCount;
	__iPassCount = __iPassCount > 4 ? 4 : __iPassCount;

	if (__iPassCount > 2) {
		// Depth buffer uses the same buffers as the color buffer, so fix PSMZ
		switch (gsGlobal->PSM) {
			case GS_PSM_CT32:  gsGlobal->PSMZ = GS_PSMZ_32;  break;
			case GS_PSM_CT24:  gsGlobal->PSMZ = GS_PSMZ_32;  break;
			case GS_PSM_CT16:  gsGlobal->PSMZ = GS_PSMZ_16;  break;
			case GS_PSM_CT16S: gsGlobal->PSMZ = GS_PSMZ_16S; break;
			default:
				printf("gsKit_hires: ERROR: invalid PSM\n");
				return;
		}
	}

#ifdef BG_DEBUG
	if (gsGlobal->Width < 1024)
		gsGlobal->Width -=  64;
	else
		gsGlobal->Width -= 128;
#endif

	// Bytes per pixel and height align for render buffers
	if ((gsGlobal->PSM == GS_PSM_CT32) || (gsGlobal->PSM == GS_PSM_CT24)) {
		iBpp = 4;
		iHeightAlign = 32;
	}
	else {
		iBpp = 2;
		iHeightAlign = 64;
	}

	// The size of a pass must be a multiple of 32 or 64 lines depending on the PSM
	iPassHeight = (gsGlobal->Height + (__iPassCount-1)) / __iPassCount;
	iPassHeight = (iPassHeight + (iHeightAlign-1)) & ~(iHeightAlign-1);

	// Double buffering is not used in multi pass rendering
	// Force double buffering off
	gsGlobal->DoubleBuffering = GS_SETTING_OFF;

	// gsKit_init_screen must only allocate 1 front buffer
	// so temporarily disable the depth buffer
	ZBuffering_backup = gsGlobal->ZBuffering;
	gsGlobal->ZBuffering = GS_SETTING_OFF;

	// This initialize the display and allocates 1 front buffer
	gsKit_init_screen(gsGlobal);

	// We split the front buffer in different passes
	// Each pass needs to be aligned, so the total alignment requirements become:
	// - __iPassCount * 32 for 32/24 bit color buffers
	// - __iPassCount * 64 for 16    bit color buffers
	if (gsKit_texture_size(gsGlobal->Width, gsGlobal->Height, gsGlobal->PSM) < gsKit_texture_size(gsGlobal->Width, iPassHeight * __iPassCount, gsGlobal->PSM)) {
		int iAlign = gsKit_texture_size(gsGlobal->Width, iPassHeight * __iPassCount, gsGlobal->PSM) - gsKit_texture_size(gsGlobal->Width, gsGlobal->Height, gsGlobal->PSM);
		printf("gsKit_hires: Padding front buffer with %dKiB \n", iAlign/1024);
		gsKit_vram_alloc(gsGlobal, iAlign, GSKIT_ALLOC_SYSBUFFER);
	}
	// first useable address for textures
	gsGlobal->TexturePointer = gsGlobal->CurrentPointer;

	// Restore depth buffer setting
	gsGlobal->ZBuffering = ZBuffering_backup;
	if (gsGlobal->ZBuffering == GS_SETTING_ON)
		gsKit_set_test(gsGlobal, GS_ZTEST_ON);

	//
	// The front buffer is split into the number of passes
	// Fill the buffer array here
	//
	iBufferline = 0;
	for (iPass = 0; iPass < __iPassCount; iPass++) {
		// Starting address of buffer
		__buffer[iPass].addr = iBufferline * gsGlobal->Width * iBpp;

		// First and last line of buffer
		__buffer[iPass].bufferline_begin = iBufferline;
		iBufferline += iPassHeight;
		if (iBufferline > gsGlobal->Height)
			iBufferline = gsGlobal->Height;
		__buffer[iPass].bufferline_end = iBufferline - 1;

		// First and last line of buffer, measured in CRTC scanlines
		__buffer[iPass].scanline_begin = __buffer[iPass].bufferline_begin;
		__buffer[iPass].scanline_end   = __buffer[iPass].bufferline_end;
		if (gsGlobal->MagV > 0)
		{
			// Lines in the buffer are magnified (displayed more than once
			__buffer[iPass].scanline_begin *= (gsGlobal->MagV+1);
			__buffer[iPass].scanline_end   *= (gsGlobal->MagV+1);
		}
		if ((gsGlobal->Interlace == GS_INTERLACED) && (gsGlobal->Field == GS_FIELD))
		{
			// Only half of the lines in the buffer are read in interlaced field mode
			__buffer[iPass].scanline_begin /= 2;
			__buffer[iPass].scanline_end   /= 2;
		}
	}

	if ((gsGlobal->ZBuffering == GS_SETTING_ON) && (__iPassCount == 2)) {
		// Depth buffer not inside front buffer, so allocate it
		// We only need half the height of the depth buffer
		gsGlobal->ZBuffer = gsKit_vram_alloc(gsGlobal, gsKit_texture_size(gsGlobal->Width, gsGlobal->Height/2, gsGlobal->PSMZ), GSKIT_ALLOC_SYSBUFFER);

		// 3rd buffer is fixed to this depth buffer
		__buffer[2].addr = gsGlobal->ZBuffer;
	}

	//
	// What buffers need to be used in what pass? That's what the pass array is for.
	// Fill the pass array
	//
	for (iPass = 0; iPass < __iPassCount; iPass++) {
		int iPrevPass = (iPass == 0) ? (__iPassCount-1) : (iPass-1);

		// We always draw top to bottom
		__pass[iPass].DrawBuffer = &__buffer[iPass];
		// The CRTC is right behind the draw buffer
		__pass[iPass].CRTCBuffer = &__buffer[iPrevPass];

		if (__iPassCount == 2) {
			// Depth buffer fixed
			__pass[iPass].DepthBuffer = &__buffer[2];
			__pass[iPass].TextBuffer  = NULL;
		}
		else if (__iPassCount == 3) {
			int iNextPass = (iPass == (__iPassCount-1)) ? 0 : (iPass+1);

			// Depth buffer inside front buffer
			__pass[iPass].DepthBuffer = &__buffer[iNextPass];
			__pass[iPass].TextBuffer  = NULL;
		}
		else {
			int iNextPass1 = (iPass == (__iPassCount-1)) ? 0 : (iPass+1);
			int iNextPass2 = (iNextPass1 == (__iPassCount-1)) ? 0 : (iNextPass1+1);

			// Depth and texture buffers inside front buffer
			//
			// Toggle the position of the buffers relative to the draw buffer
			// so the texture buffer can stay in place twice as long
			__pass[iPass].DepthBuffer = &__buffer[((iPass & 1) == 0) ? iNextPass1 : iNextPass2];
			__pass[iPass].TextBuffer  = &__buffer[((iPass & 1) == 0) ? iNextPass2 : iNextPass1];
		}
	}

	// (Re-)create pass queues
	_gsKit_create_passes(gsGlobal);

	// Re-use the "per" queue from gsKit
	__DrawQueue[0] = *gsGlobal->Per_Queue;
	// Create a second identical queue for double buffering
	gsKit_queue_init(gsGlobal, &__DrawQueue[1], GS_PERSISTENT, GS_RENDER_QUEUE_PER_POOLSIZE);
	// Set active queue
	gsKit_queue_set(gsGlobal, &__DrawQueue[__iCurrentDrawQueue]);

	// Initialize semaphores
	sema.init_count = 0;
	sema.max_count = 1;
	sema.option = 0;
	if ((__sema_hsync_id = CreateSema(&sema)) < 0)
		printf("gsKit_hires: ERROR: creating __sema_hsync_id\n");
	sema.init_count = 0;
	sema.max_count = 1;
	sema.option = 0;
	if ((__sema_vsync_id = CreateSema(&sema)) < 0)
		printf("gsKit_hires: ERROR: creating __sema_vsync_id\n");
	// Initialize mutex (also a semaphore)
	sema.init_count = 1;
	sema.max_count = 1;
	sema.option = 0;
	if ((__sema_queue_id = CreateSema(&sema)) < 0)
		printf("gsKit_hires: ERROR: creating __sema_queue_id\n");

	// Create thread for transmitting the draw queues to the GS
	// This thread must have the highest priority (1).
	thread.initial_priority	= 1;
	thread.stack_size		= STACK_SIZE;
	thread.gp_reg			= &_gp;
	thread.func				= (void *)dma_to_gs_thread;
	thread.stack			= (void *)__ThreadStack;
	thread.option			= 0;
	thread.attr				= 0;
	if ((__ThreadId = CreateThread(&thread)) < 0)
		printf("gsKit_hires: ERROR: creating thread\n");
	StartThread(__ThreadId, gsGlobal);

	// HACK: Change the priority of the caller,
	// so we can have the highest priority.
	ChangeThreadPriority(GetThreadId(), 2);

	// Install hsync handler
	// This will start firing events (using semaphores) to the GS
	if ((__hsync_callback_id = gsKit_add_hsync_handler(hsync_callback)) < 0)
		printf("gsKit_hires: ERROR: registering hsync handler\n");
}
#endif

#if F_gsKit_hires_init_global
GSGLOBAL *gsKit_hires_init_global()
{
	return gsKit_init_global();
}
#endif

#if F_gsKit_hires_deinit_global
void gsKit_hires_deinit_global(GSGLOBAL *gsGlobal)
{
	u32 iPass;

	if (__hsync_callback_id >= 0)
		gsKit_remove_hsync_handler(__hsync_callback_id);

	WaitSema(__sema_queue_id);

	TerminateThread(__ThreadId);
	DeleteThread(__ThreadId);

	if (__sema_queue_id >= 0)
		DeleteSema(__sema_queue_id);

	if (__sema_vsync_id >= 0)
		DeleteSema(__sema_vsync_id);

	if (__sema_hsync_id >= 0)
		DeleteSema(__sema_hsync_id);

	dmaKit_wait_fast();
	gsKit_queue_free(gsGlobal, &__DrawQueue[1]);

	for (iPass = 0; iPass < __iPassCount; iPass++) {
		gsKit_queue_free(gsGlobal, &__pass[iPass].Queue);
		gsKit_queue_free(gsGlobal, &__pass[iPass].QueueIF);
	}

	gsKit_deinit_global(gsGlobal);
}
#endif
