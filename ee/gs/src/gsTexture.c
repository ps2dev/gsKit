//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsTexture.c - Texture loading, handling, and manipulation.
//
// lzw() implimentation from jbit! Thanks jbit!
//

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <kernel.h>

#include "gsKit.h"
#include "gsInline.h"

static inline u32 lzw(u32 val)
{
	u32 res;
	__asm__ __volatile__ ("   plzcw   %0, %1    " : "=r" (res) : "r" (val));
	return(res);
}

u32  gsKit_texture_size_ee(int width, int height, int psm)
{

#ifdef DEBUG
	printf("Width: %d - Height: %d\n", width, height);
#endif

	switch (psm) {
		case GS_PSM_CT32:  return (width*height*4);
		case GS_PSM_CT24:  return (width*height*4);
		case GS_PSM_CT16:  return (width*height*2);
		case GS_PSM_CT16S: return (width*height*2);
		case GS_PSM_T8:    return (width*height  );
		case GS_PSM_T4:    return (width*height/2);
		default: printf("gsKit: unsupported PSM %d\n", psm);
	}

	return -1;
}

u32  gsKit_texture_size(int width, int height, int psm)
{
	int widthBlocks, heightBlocks;
	int widthAlign, heightAlign;

	// Calculate the number of blocks width and height
	// A block is 256 bytes in size
	switch (psm) {
		case GS_PSM_CT32:
		case GS_PSM_CT24:
			// 1 block = 8x8 pixels
			widthBlocks  = (width  + 7) / 8;
			heightBlocks = (height + 7) / 8;
			break;
		case GS_PSM_CT16:
		case GS_PSM_CT16S:
			// 1 block = 16x8 pixels
			widthBlocks  = (width  + 15) / 16;
			heightBlocks = (height +  7) /  8;
			break;
		case GS_PSM_T8:
			// 1 block = 16x16 pixels
			widthBlocks  = (width  + 15) / 16;
			heightBlocks = (height + 15) / 16;
			break;
		case GS_PSM_T4:
			// 1 block = 32x16 pixels
			widthBlocks  = (width  + 31) / 32;
			heightBlocks = (height + 15) / 16;
			break;
		default:
			printf("gsKit: unsupported PSM %d\n", psm);
			return -1;
	}

	// Calculate the minimum block alignment
	if(psm == GS_PSM_CT32 || psm == GS_PSM_CT24 || psm == GS_PSM_T8) {
		// 8x4 blocks in a page.
		// block traversing order:
		// 0145....
		// 2367....
		// ........
		// ........
		if(widthBlocks <= 2 && heightBlocks <= 1) {
			widthAlign = 1;
			heightAlign = 1;
		}
		else if(widthBlocks <= 4 && heightBlocks <= 2) {
			widthAlign = 2;
			heightAlign = 2;
		}
		else if(widthBlocks <= 8 && heightBlocks <= 4) {
			widthAlign = 4;
			heightAlign = 4;
		}
		else {
			widthAlign = 8;
			heightAlign = 4;
		}
	}
	else if(psm == GS_PSM_CT16 || psm == GS_PSM_T4) {
		// 4x8 blocks in a page.
		// block traversing order:
		// 02..
		// 13..
		// 46..
		// 57..
		// ....
		// ....
		// ....
		// ....
		if(widthBlocks <= 1 && heightBlocks <= 2) {
			widthAlign = 1;
			heightAlign = 1;
		}
		else if(widthBlocks <= 2 && heightBlocks <= 4) {
			widthAlign = 2;
			heightAlign = 2;
		}
		else if(widthBlocks <= 4 && heightBlocks <= 8) {
			widthAlign = 4;
			heightAlign = 4;
		}
		else {
			widthAlign = 4;
			heightAlign = 8;
		}
	}
	else /* if(psm == GS_PSM_CT16S) */ {
		// 4x8 blocks in a page.
		// block traversing order:
		// 02..
		// 13..
		// ....
		// ....
		// 46..
		// 57..
		// ....
		// ....
		if(widthBlocks <= 1 && heightBlocks <= 2) {
			widthAlign = 1;
			heightAlign = 1;
		}
		else if(widthBlocks <= 2 && heightBlocks <= 2) {
			widthAlign = 2;
			heightAlign = 2;
		}
		else if(widthBlocks <= 2 && heightBlocks <= 8) {
			widthAlign = 2;
			heightAlign = 8;
		}
		else {
			widthAlign = 4;
			heightAlign = 8;
		}
	}

	widthBlocks  = (-widthAlign)  & (widthBlocks  + widthAlign  - 1);
	heightBlocks = (-heightAlign) & (heightBlocks + heightAlign - 1);

	return widthBlocks * heightBlocks * 256;
}

static inline u8
from8to5(u8 c, const s8 dither)
{
	if((c+dither) > 255)
		return 255>>3;
	else if((c+dither) < 0)
		return 0;
	else
		return (c+dither)>>3;
}

static inline u16
fromRGBto16(const s8 dither, u8 r, u8 g, u8 b)
{
	// A1 B5 G5 R5
	return 0x8000 | (from8to5(b, dither)<<10) | (from8to5(g, dither)<<5) | from8to5(r, dither);
}

static inline u16
from24to16(const s8 dither, u8 *c)
{
	return fromRGBto16(dither, c[0], c[1], c[2]);
}

void gsKit_texture_to_psm16(GSTEXTURE *Texture)
{
	int x, y;
	const s8 dither_matrix[16] = {-4,2,-3,3,0,-2,1,-1,-3,3,-4,2,1,-1,0,-2};

	// Only 24bit to 16bit supported
	if (Texture->PSM != GS_PSM_CT24)
		return;

	size_t size = Texture->Width * Texture->Height * 2;
	u16 *pixels16 = (u16*)memalign(128, size);
	u8  *pixels24 = (u8 *)Texture->Mem;

	for(y=0; y < Texture->Height; y++) {
		for(x=0; x < Texture->Width; x++) {
			int i = y * Texture->Width + x;
			s8 dither = dither_matrix[(y&3)*4+(x&3)];
			pixels16[i] = from24to16(dither, &pixels24[i*3]);
		}
	}

	free(Texture->Mem);
	Texture->Mem = (void*)pixels16;
	Texture->PSM = GS_PSM_CT16S;
}

void gsKit_texture_send(u32 *mem, int width, int height, u32 tbp, u32 psm, u32 tbw, u8 clut)
{
	u64* p_store;
	u64* p_data;
	u32* p_mem;
	int packets;
	int remain;
	int qwc;
	int p_size;

	qwc = gsKit_texture_size_ee(width, height, psm) / 16;
	if( gsKit_texture_size_ee(width, height, psm) % 16 )
	{
#ifdef GSKIT_DEBUG
		printf("Uneven division of quad word count from VRAM alloc. Rounding up QWC.\n");
#endif
		qwc++;
	}

	packets = qwc / GS_GIF_BLOCKSIZE;
	remain  = qwc % GS_GIF_BLOCKSIZE;
	p_mem   = (u32*)mem;

	if(clut == GS_CLUT_TEXTURE)
		p_size = (7+(packets * 3)+remain);
	else
		p_size = (9+(packets * 3)+remain);

	if(remain > 0)
		p_size += 2;

	p_store = p_data = memalign(64, (p_size * 16));

	FlushCache(0);

	// DMA DATA
	*p_data++ = DMA_TAG( 5, 0, DMA_CNT, 0, 0, 0 );
	*p_data++ = 0;

	*p_data++ = GIF_TAG( 4, 0, 0, 0, 0, 1 );
	*p_data++ = GIF_AD;

#ifdef GSKIT_DEBUG
	if(tbp % 256)
		printf("ERROR: Texture base pointer not on 256 byte alignment - Modulo = %d\n", tbp % 256);
#endif

	*p_data++ = GS_SETREG_BITBLTBUF(0, 0, 0, tbp/256, tbw, psm);
	*p_data++ = GS_BITBLTBUF;

	*p_data++ = GS_SETREG_TRXPOS(0, 0, 0, 0, 0);
	*p_data++ = GS_TRXPOS;

	*p_data++ = GS_SETREG_TRXREG(width, height);
	*p_data++ = GS_TRXREG;

	*p_data++ = GS_SETREG_TRXDIR(0);
	*p_data++ = GS_TRXDIR;

	while (packets-- > 0)
	{
		*p_data++ = DMA_TAG( 1, 0, DMA_CNT, 0, 0, 0);
		*p_data++ = 0;
		*p_data++ = GIF_TAG( GS_GIF_BLOCKSIZE, 0, 0, 0, 2, 0 );
		*p_data++ = 0;
		*p_data++ = DMA_TAG( GS_GIF_BLOCKSIZE, 1, DMA_REF, 0, (u32)p_mem, 0 );
		*p_data++ = 0;
		p_mem+= (GS_GIF_BLOCKSIZE * 4);
	}

	if (remain > 0) {
		*p_data++ = DMA_TAG( 1, 0, DMA_CNT, 0, 0, 0);
		*p_data++ = 0;
		*p_data++ = GIF_TAG( remain, 0, 0, 0, 2, 0 );
		*p_data++ = 0;
		*p_data++ = DMA_TAG( remain, 1, DMA_REF, 0, (u32)p_mem, 0 );
		*p_data++ = 0;
	}

	if(clut == GS_CLUT_TEXTURE)
	{
		*p_data++ = DMA_TAG( 0, 0, DMA_END, 0, 0, 0 );
		*p_data++ = 0;
	}
	else
	{
		*p_data++ = DMA_TAG( 2, 0, DMA_END, 0, 0, 0 );
		*p_data++ = 0;

		*p_data++ = GIF_TAG( 1, 1, 0, 0, 0, 1 );
		*p_data++ = GIF_AD;

		*p_data++ = 0;
		*p_data++ = GS_TEXFLUSH;
	}

	// Need to wait first to make sure that if our doublebuffered drawbuffer is still
	// chugging away we have a safe time to start our transfer.
	dmaKit_wait_fast();
	dmaKit_send_chain(DMA_CHANNEL_GIF, p_store, p_size);
	dmaKit_wait_fast();	//Wait for the DMA transfer to complete, before freeing the GIF packet buffer.
	free(p_store);
}

void gsKit_texture_send_inline(GSGLOBAL *gsGlobal, u32 *mem, int width, int height, u32 tbp, u32 psm, u32 tbw, u8 clut)
{
	u64* p_data;
	u32* p_mem;
	int packets;
	int remain;
	int qwc;
	int dmasize;

	qwc = gsKit_texture_size_ee(width, height, psm) / 16;
	if( gsKit_texture_size_ee(width, height, psm) % 16 )
	{
#ifdef GSKIT_DEBUG
		printf("Uneven division of quad word count from VRAM alloc. Rounding up QWC.\n");
#endif
		qwc++;
	}

	packets = qwc / GS_GIF_BLOCKSIZE;
	remain  = qwc % GS_GIF_BLOCKSIZE;
	p_mem   = (u32*)mem;

	p_data = gsKit_heap_alloc(gsGlobal, 4, 64, GIF_AD);

	*p_data++ = GIF_TAG_AD(4);
	*p_data++ = GIF_AD;

#ifdef GSKIT_DEBUG
	if(tbp % 256)
		printf("ERROR: Texture base pointer not on 256 byte alignment - Modulo = %d\n", tbp % 256);
#endif

	*p_data++ = GS_SETREG_BITBLTBUF(0, 0, 0, tbp/256, tbw, psm);
	*p_data++ = GS_BITBLTBUF;

	*p_data++ = GS_SETREG_TRXPOS(0, 0, 0, 0, 0);
	*p_data++ = GS_TRXPOS;

	*p_data++ = GS_SETREG_TRXREG(width, height);
	*p_data++ = GS_TRXREG;

	*p_data++ = GS_SETREG_TRXDIR(0);
	*p_data++ = GS_TRXDIR;

	dmasize = (packets * 3);
	if(remain)
		dmasize += 3;

	p_data = gsKit_heap_alloc_dma(gsGlobal, dmasize, dmasize * 16);

	while (packets-- > 0)
	{
		*p_data++ = DMA_TAG( 1, 0, DMA_CNT, 0, 0, 0);
		*p_data++ = 0;
		*p_data++ = GIF_TAG( GS_GIF_BLOCKSIZE, 0, 0, 0, 2, 0 );
		*p_data++ = 0;
		*p_data++ = DMA_TAG( GS_GIF_BLOCKSIZE, 0, DMA_REF, 0, (u32)p_mem, 0 );
		*p_data++ = 0;
		p_mem+= (GS_GIF_BLOCKSIZE * 4);
	}

	if (remain > 0) {
		*p_data++ = DMA_TAG( 1, 0, DMA_CNT, 0, 0, 0);
		*p_data++ = 0;
		*p_data++ = GIF_TAG( remain, 0, 0, 0, 2, 0 );
		*p_data++ = 0;
		*p_data++ = DMA_TAG( remain, 0, DMA_REF, 0, (u32)p_mem, 0 );
		*p_data++ = 0;
	}

	if(clut != GS_CLUT_TEXTURE)
	{
		p_data = gsKit_heap_alloc(gsGlobal, 1, 16, GIF_AD);

		*p_data++ = GIF_TAG_AD(1);
		*p_data++ = GIF_AD;

		*p_data++ = 0;
		*p_data++ = GS_TEXFLUSH;
	}
}

void gsKit_texture_upload(GSGLOBAL *gsGlobal, GSTEXTURE *Texture)
{
	gsKit_setup_tbw(Texture);

	if (Texture->PSM == GS_PSM_T8)
	{
		gsKit_texture_send(Texture->Mem, Texture->Width, Texture->Height, Texture->Vram, Texture->PSM, Texture->TBW, GS_CLUT_TEXTURE);
		gsKit_texture_send(Texture->Clut, 16, 16, Texture->VramClut, Texture->ClutPSM, 1, GS_CLUT_PALLETE);

	}
	else if (Texture->PSM == GS_PSM_T4)
	{
		gsKit_texture_send(Texture->Mem, Texture->Width, Texture->Height, Texture->Vram, Texture->PSM, Texture->TBW, GS_CLUT_TEXTURE);
		gsKit_texture_send(Texture->Clut, 8,  2, Texture->VramClut, Texture->ClutPSM, 1, GS_CLUT_PALLETE);
	}
	else
	{
		gsKit_texture_send(Texture->Mem, Texture->Width, Texture->Height, Texture->Vram, Texture->PSM, Texture->TBW, GS_CLUT_NONE);
	}
}

static inline void gsKit_set_tw_th(const GSTEXTURE *Texture, int *tw, int *th)
{
	*tw = 31 - (lzw(Texture->Width) + 1);
	if(Texture->Width > (1<<*tw))
		(*tw)++;

	*th = 31 - (lzw(Texture->Height) + 1);
	if(Texture->Height > (1<<*th))
		(*th)++;
}

void gsKit_prim_sprite_texture_3d(GSGLOBAL *gsGlobal, const GSTEXTURE *Texture,
				float x1, float y1, int iz1, float u1, float v1,
				float x2, float y2, int iz2, float u2, float v2, u64 color)
{
	gsKit_set_texfilter(gsGlobal, Texture->Filter);

	u64* p_store;
	u64* p_data;
	int qsize = 4;
	int bsize = 64;

	int tw, th;
	gsKit_set_tw_th(Texture, &tw, &th);

	int ix1 = gsKit_float_to_int_x(gsGlobal, x1);
	int ix2 = gsKit_float_to_int_x(gsGlobal, x2);
	int iy1 = gsKit_float_to_int_y(gsGlobal, y1);
	int iy2 = gsKit_float_to_int_y(gsGlobal, y2);

	int iu1 = gsKit_float_to_int_u(Texture, u1);
	int iu2 = gsKit_float_to_int_u(Texture, u2);
	int iv1 = gsKit_float_to_int_v(Texture, v1);
	int iv2 = gsKit_float_to_int_v(Texture, v2);

	p_store = p_data = gsKit_heap_alloc(gsGlobal, qsize, bsize, GSKIT_GIF_PRIM_SPRITE_TEXTURED);

	*p_data++ = GIF_TAG_SPRITE_TEXTURED(0);
	*p_data++ = GIF_TAG_SPRITE_TEXTURED_REGS(gsGlobal->PrimContext);

	if(Texture->VramClut == 0)
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			0, 0, 0, 0, GS_CLUT_STOREMODE_NOLOAD);
	}
	else
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			Texture->VramClut/256, Texture->ClutPSM, 0, 0, GS_CLUT_STOREMODE_LOAD);
	}

	*p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_SPRITE, 0, 1, gsGlobal->PrimFogEnable,
				gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
				1, gsGlobal->PrimContext, 0);

	*p_data++ = color;

	*p_data++ = GS_SETREG_UV( iu1, iv1 );
	*p_data++ = GS_SETREG_XYZ2( ix1, iy1, iz1 );

	*p_data++ = GS_SETREG_UV( iu2, iv2 );
	*p_data++ = GS_SETREG_XYZ2( ix2, iy2, iz2 );
}

void gsKit_prim_sprite_striped_texture_3d(GSGLOBAL *gsGlobal, const GSTEXTURE *Texture,
				float x1, float y1, int iz1, float u1, float v1,
				float x2, float y2, int iz2, float u2, float v2, u64 color)
{
	// If you do bilinear on this the results will be a disaster because of bleeding.
	gsKit_set_texfilter(gsGlobal, GS_FILTER_NEAREST);

	u64* p_store;
	u64* p_data;
	int qsize = 4;
	int bsize = 64;

	int ix1 = x1;
	int ix2 = x2;
	int iy1 = gsKit_float_to_int_y(gsGlobal, y1);
	int iy2 = gsKit_float_to_int_y(gsGlobal, y2);

	int iv1 = gsKit_float_to_int_v(Texture, v1);
	int iv2 = gsKit_float_to_int_v(Texture, v2);

	int tw, th;
	gsKit_set_tw_th(Texture, &tw, &th);

	u64 Tex0;
	if(Texture->VramClut == 0)
	{
		Tex0 = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			0, 0, 0, 0, GS_CLUT_STOREMODE_NOLOAD);
	}
	else
	{
		Tex0 = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			Texture->VramClut/256, Texture->ClutPSM, 0, 0, GS_CLUT_STOREMODE_LOAD);
	}

	u64 Prim = GS_SETREG_PRIM( GS_PRIM_PRIM_SPRITE, 0, 1, gsGlobal->PrimFogEnable,
				gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
				1, gsGlobal->PrimContext, 0);

	ix1 = ((ix1 >> 6) << 6);
	ix2 = ((ix2 >> 6) << 6);

	int extracount = 0;
	if(((int)x1) % 64)
	{
		ix1 += 64;
		extracount++;
	}
	if(((int)x2) % 64)
	{
		extracount++;
	}

	int stripcount = ((ix2 - ix1) / 64);

	if(!(stripcount + extracount))
	{
		return;
	}

	float leftwidth = ix1 - (float)x1;
	float rightwidth = (float)x2 - ix2;

	float ustripwidth;

	float leftuwidth = ((leftwidth / (x2-x1)) * (u2 - u1));

	float rightuwidth = ((rightwidth / (x2-x1)) * (u2 - u1));

	ustripwidth = ((((u2 - u1) - leftuwidth) - rightuwidth) / stripcount);

	float fu1 = 0.0f;

	// This is a lie btw... it's not really A+D mode, but I lie to my allocator so it doesn't screw up the NLOOP arg of the GIFTAG.
	p_store = p_data = gsKit_heap_alloc(gsGlobal, qsize * (stripcount + extracount), bsize * (stripcount + extracount), GIF_AD);

	*p_data++ = GIF_TAG_SPRITE_TEXTURED((stripcount + extracount));
	*p_data++ = GIF_TAG_SPRITE_TEXTURED_REGS(gsGlobal->PrimContext);

	//printf("stripcount = %d | extracount = %d | ustripwidth = %f | rightwidth = %f\n", stripcount, extracount, ustripwidth, rightwidth);

	if(leftwidth >= 1.0f)
	{
		*p_data++ = Tex0;
		*p_data++ = Prim;

		*p_data++ = color;

		*p_data++ = GS_SETREG_UV( gsKit_float_to_int_u(Texture, u1), iv1 );
		*p_data++ = GS_SETREG_XYZ2( gsKit_float_to_int_x(gsGlobal, x1), iy1, iz1 );

		fu1 = leftuwidth;

		*p_data++ = GS_SETREG_UV( gsKit_float_to_int_u(Texture, fu1), iv2 );
		*p_data++ = GS_SETREG_XYZ2( (ix1 << 4) + gsGlobal->OffsetX, iy2, iz2 );

		*p_data++ = 0;
	}

	while(stripcount--)
	{
		*p_data++ = Tex0;
		*p_data++ = Prim;

		*p_data++ = color;

		*p_data++ = GS_SETREG_UV( gsKit_float_to_int_u(Texture, fu1), iv1 );
		*p_data++ = GS_SETREG_XYZ2( (ix1 << 4) + gsGlobal->OffsetX, iy1, iz1 );

		fu1 += ustripwidth;
		ix1 += 64;

		*p_data++ = GS_SETREG_UV( gsKit_float_to_int_u(Texture, fu1), iv2 );
		*p_data++ = GS_SETREG_XYZ2( (ix1 << 4) + gsGlobal->OffsetX, iy2, iz2 );

		*p_data++ = 0;
	}


	if(rightwidth > 0.0f)
	{

		*p_data++ = Tex0;
		*p_data++ = Prim;

		*p_data++ = color;

		*p_data++ = GS_SETREG_UV( gsKit_float_to_int_u(Texture, fu1), iv1 );
		*p_data++ = GS_SETREG_XYZ2( (ix1 << 4) + gsGlobal->OffsetX, iy1, iz1 );

		fu1 += rightuwidth;
		rightwidth += ix1;

		*p_data++ = GS_SETREG_UV( gsKit_float_to_int_u(Texture, fu1), iv2 );
		*p_data++ = GS_SETREG_XYZ2( gsKit_float_to_int_x(gsGlobal, rightwidth), iy2, iz2 );

		*p_data++ = 0;

	}
}

void gsKit_prim_triangle_texture_3d(GSGLOBAL *gsGlobal, GSTEXTURE *Texture,
				float x1, float y1, int iz1, float u1, float v1,
				float x2, float y2, int iz2, float u2, float v2,
				float x3, float y3, int iz3, float u3, float v3, u64 color)
{
	gsKit_set_texfilter(gsGlobal, Texture->Filter);
	u64* p_store;
	u64* p_data;
	int qsize = 5;
	int bsize = 80;

	int tw, th;
	gsKit_set_tw_th(Texture, &tw, &th);

	int ix1 = gsKit_float_to_int_x(gsGlobal, x1);
	int ix2 = gsKit_float_to_int_x(gsGlobal, x2);
	int ix3 = gsKit_float_to_int_x(gsGlobal, x3);
	int iy1 = gsKit_float_to_int_y(gsGlobal, y1);
	int iy2 = gsKit_float_to_int_y(gsGlobal, y2);
	int iy3 = gsKit_float_to_int_y(gsGlobal, y3);

	int iu1 = gsKit_float_to_int_u(Texture, u1);
	int iu2 = gsKit_float_to_int_u(Texture, u2);
	int iu3 = gsKit_float_to_int_u(Texture, u3);
	int iv1 = gsKit_float_to_int_v(Texture, v1);
	int iv2 = gsKit_float_to_int_v(Texture, v2);
	int iv3 = gsKit_float_to_int_v(Texture, v3);

	p_store = p_data = gsKit_heap_alloc(gsGlobal, qsize, bsize, GSKIT_GIF_PRIM_TRIANGLE_TEXTURED);

	*p_data++ = GIF_TAG_TRIANGLE_TEXTURED(0);
	*p_data++ = GIF_TAG_TRIANGLE_TEXTURED_REGS(gsGlobal->PrimContext);

	if(Texture->VramClut == 0)
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			0, 0, 0, 0, GS_CLUT_STOREMODE_NOLOAD);
	}
	else
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			Texture->VramClut/256, Texture->ClutPSM, 0, 0, GS_CLUT_STOREMODE_LOAD);
	}

	*p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_TRIANGLE, 0, 1, gsGlobal->PrimFogEnable,
				gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
				1, gsGlobal->PrimContext, 0);


	*p_data++ = color;

	*p_data++ = GS_SETREG_UV( iu1, iv1 );
	*p_data++ = GS_SETREG_XYZ2( ix1, iy1, iz1 );

	*p_data++ = GS_SETREG_UV( iu2, iv2 );
	*p_data++ = GS_SETREG_XYZ2( ix2, iy2, iz2 );

	*p_data++ = GS_SETREG_UV( iu3, iv3 );
	*p_data++ = GS_SETREG_XYZ2( ix3, iy3, iz3 );
}

void gsKit_prim_triangle_goraud_texture_3d(GSGLOBAL *gsGlobal, GSTEXTURE *Texture,
				float x1, float y1, int iz1, float u1, float v1,
				float x2, float y2, int iz2, float u2, float v2,
				float x3, float y3, int iz3, float u3, float v3,
				u64 color1, u64 color2, u64 color3)
{
	gsKit_set_texfilter(gsGlobal, Texture->Filter);
	u64* p_store;
	u64* p_data;
	int qsize = 6;
	int bsize = 96;

	int tw, th;
	gsKit_set_tw_th(Texture, &tw, &th);

	int ix1 = gsKit_float_to_int_x(gsGlobal, x1);
	int ix2 = gsKit_float_to_int_x(gsGlobal, x2);
	int ix3 = gsKit_float_to_int_x(gsGlobal, x3);
	int iy1 = gsKit_float_to_int_y(gsGlobal, y1);
	int iy2 = gsKit_float_to_int_y(gsGlobal, y2);
	int iy3 = gsKit_float_to_int_y(gsGlobal, y3);

	int iu1 = gsKit_float_to_int_u(Texture, u1);
	int iu2 = gsKit_float_to_int_u(Texture, u2);
	int iu3 = gsKit_float_to_int_u(Texture, u3);
	int iv1 = gsKit_float_to_int_v(Texture, v1);
	int iv2 = gsKit_float_to_int_v(Texture, v2);
	int iv3 = gsKit_float_to_int_v(Texture, v3);

	p_store = p_data = gsKit_heap_alloc(gsGlobal, qsize, bsize, GSKIT_GIF_PRIM_TRIANGLE_TEXTURED);

	*p_data++ = GIF_TAG_TRIANGLE_GORAUD_TEXTURED(0);
	*p_data++ = GIF_TAG_TRIANGLE_GORAUD_TEXTURED_REGS(gsGlobal->PrimContext);

	if(Texture->VramClut == 0)
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			0, 0, 0, 0, GS_CLUT_STOREMODE_NOLOAD);
	}
	else
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			Texture->VramClut/256, Texture->ClutPSM, 0, 0, GS_CLUT_STOREMODE_LOAD);
	}

	*p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_TRIANGLE, 1, 1, gsGlobal->PrimFogEnable,
				gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
				1, gsGlobal->PrimContext, 0);


	*p_data++ = color1;
	*p_data++ = GS_SETREG_UV( iu1, iv1 );
	*p_data++ = GS_SETREG_XYZ2( ix1, iy1, iz1 );

	*p_data++ = color2;
	*p_data++ = GS_SETREG_UV( iu2, iv2 );
	*p_data++ = GS_SETREG_XYZ2( ix2, iy2, iz2 );

	*p_data++ = color3;
	*p_data++ = GS_SETREG_UV( iu3, iv3 );
	*p_data++ = GS_SETREG_XYZ2( ix3, iy3, iz3 );
}

void gsKit_prim_triangle_strip_texture(GSGLOBAL *gsGlobal, GSTEXTURE *Texture,
					float *TriStrip, int segments, int iz, u64 color)
{
	gsKit_set_texfilter(gsGlobal, Texture->Filter);
	u64* p_store;
	u64* p_data;
	int qsize = 3 + (segments * 2);
	int count;
	int vertexdata[segments*4];

	int tw, th;
	gsKit_set_tw_th(Texture, &tw, &th);

	for(count = 0; count < (segments * 4); count+=4)
	{
		vertexdata[count+0] = gsKit_float_to_int_x(gsGlobal, *TriStrip++);
		vertexdata[count+1] = gsKit_float_to_int_y(gsGlobal, *TriStrip++);
		vertexdata[count+2] = gsKit_float_to_int_u(Texture,  *TriStrip++);
		vertexdata[count+3] = gsKit_float_to_int_v(Texture,  *TriStrip++);
	}

	p_store = p_data = gsKit_heap_alloc(gsGlobal, qsize, (qsize * 16), GIF_AD);

	*p_data++ = GIF_TAG_AD(qsize);
	*p_data++ = GIF_AD;

	if(Texture->VramClut == 0)
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			0, 0, 0, 0, GS_CLUT_STOREMODE_NOLOAD);
	}
	else
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			Texture->VramClut/256, Texture->ClutPSM, 0, 0, GS_CLUT_STOREMODE_LOAD);
	}
	*p_data++ = GS_TEX0_1+gsGlobal->PrimContext;

	*p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_TRISTRIP, 0, 1, gsGlobal->PrimFogEnable,
				gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
				1, gsGlobal->PrimContext, 0);

	*p_data++ = GS_PRIM;

	*p_data++ = color;
	*p_data++ = GS_RGBAQ;

	for(count = 0; count < (segments * 4); count+=4)
	{
		*p_data++ = GS_SETREG_UV( vertexdata[count+2], vertexdata[count+3] );
		*p_data++ = GS_UV;

		*p_data++ = GS_SETREG_XYZ2( vertexdata[count], vertexdata[count+1], iz );
		*p_data++ = GS_XYZ2;
	}
}

void gsKit_prim_triangle_strip_texture_3d(GSGLOBAL *gsGlobal, GSTEXTURE *Texture,
					float *TriStrip, int segments, u64 color)
{
	gsKit_set_texfilter(gsGlobal, Texture->Filter);
	u64* p_store;
	u64* p_data;
	int qsize = 3 + (segments * 2);
	int count;
	int vertexdata[segments*5];

	int tw, th;
	gsKit_set_tw_th(Texture, &tw, &th);

	for(count = 0; count < (segments * 5); count+=5)
	{
		vertexdata[count+0] = gsKit_float_to_int_x(gsGlobal, *TriStrip++);
		vertexdata[count+1] = gsKit_float_to_int_y(gsGlobal, *TriStrip++);
		vertexdata[count+2] = (int)((*TriStrip++) * 16.0f); // z
		vertexdata[count+3] = gsKit_float_to_int_u(Texture,  *TriStrip++);
		vertexdata[count+4] = gsKit_float_to_int_v(Texture,  *TriStrip++);
	}

	p_store = p_data = gsKit_heap_alloc(gsGlobal, qsize, (qsize * 16), GIF_AD);

	*p_data++ = GIF_TAG_AD(qsize);
	*p_data++ = GIF_AD;

	if(Texture->VramClut == 0)
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			0, 0, 0, 0, GS_CLUT_STOREMODE_NOLOAD);
	}
	else
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			Texture->VramClut/256, Texture->ClutPSM, 0, 0, GS_CLUT_STOREMODE_LOAD);
	}
	*p_data++ = GS_TEX0_1+gsGlobal->PrimContext;

	*p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_TRISTRIP, 0, 1, gsGlobal->PrimFogEnable,
				gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
				1, gsGlobal->PrimContext, 0);

	*p_data++ = GS_PRIM;

	*p_data++ = color;
	*p_data++ = GS_RGBAQ;

	for(count = 0; count < (segments * 5); count+=5)
	{
		*p_data++ = GS_SETREG_UV( vertexdata[count+3], vertexdata[count+4] );
		*p_data++ = GS_UV;

		*p_data++ = GS_SETREG_XYZ2( vertexdata[count], vertexdata[count+1], vertexdata[count+2] );
		*p_data++ = GS_XYZ2;
	}
}

void gsKit_prim_triangle_fan_texture(GSGLOBAL *gsGlobal, GSTEXTURE *Texture,
					float *TriFan, int verticies, int iz, u64 color)
{
	gsKit_set_texfilter(gsGlobal, Texture->Filter);
	u64* p_store;
	u64* p_data;
	int qsize = 3 + (verticies * 2);
	int count;
	int vertexdata[verticies*4];

	int tw, th;
	gsKit_set_tw_th(Texture, &tw, &th);

	for(count = 0; count < (verticies * 4); count+=4)
	{
		vertexdata[count+0] = gsKit_float_to_int_x(gsGlobal, *TriFan++);
		vertexdata[count+1] = gsKit_float_to_int_y(gsGlobal, *TriFan++);
		vertexdata[count+2] = gsKit_float_to_int_u(Texture,  *TriFan++);
		vertexdata[count+3] = gsKit_float_to_int_v(Texture,  *TriFan++);
	}

	p_store = p_data = gsKit_heap_alloc(gsGlobal, qsize, (qsize * 16), GIF_AD);

	*p_data++ = GIF_TAG_AD(qsize);
	*p_data++ = GIF_AD;

	if(Texture->VramClut == 0)
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			0, 0, 0, 0, GS_CLUT_STOREMODE_NOLOAD);
	}
	else
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			Texture->VramClut/256, Texture->ClutPSM, 0, 0, GS_CLUT_STOREMODE_LOAD);
	}
	*p_data++ = GS_TEX0_1+gsGlobal->PrimContext;

	*p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_TRIFAN, 0, 1, gsGlobal->PrimFogEnable,
				gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
				1, gsGlobal->PrimContext, 0);

	*p_data++ = GS_PRIM;

	*p_data++ = color;
	*p_data++ = GS_RGBAQ;

	for(count = 0; count < (verticies * 4); count+=4)
	{
		*p_data++ = GS_SETREG_UV( vertexdata[count+2], vertexdata[count+3] );
		*p_data++ = GS_UV;

		*p_data++ = GS_SETREG_XYZ2( vertexdata[count], vertexdata[count+1], iz );
		*p_data++ = GS_XYZ2;
	}
}

void gsKit_prim_triangle_fan_texture_3d(GSGLOBAL *gsGlobal, GSTEXTURE *Texture,
					float *TriFan, int verticies, u64 color)
{
	gsKit_set_texfilter(gsGlobal, Texture->Filter);
	u64* p_store;
	u64* p_data;
	int qsize = 3 + (verticies * 2);
	int count;
	int vertexdata[verticies*5];

	int tw, th;
	gsKit_set_tw_th(Texture, &tw, &th);

	for(count = 0; count < (verticies * 5); count+=5)
	{
		vertexdata[count+0] = gsKit_float_to_int_x(gsGlobal, *TriFan++);
		vertexdata[count+1] = gsKit_float_to_int_y(gsGlobal, *TriFan++);
		vertexdata[count+2] = (int)((*TriFan++) * 16.0f); // z
		vertexdata[count+3] = gsKit_float_to_int_u(Texture,  *TriFan++);
		vertexdata[count+4] = gsKit_float_to_int_v(Texture,  *TriFan++);
	}

	p_store = p_data = gsKit_heap_alloc(gsGlobal, qsize, (qsize * 16), GIF_AD);

	*p_data++ = GIF_TAG_AD(qsize);
	*p_data++ = GIF_AD;

	if(Texture->VramClut == 0)
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			0, 0, 0, 0, GS_CLUT_STOREMODE_NOLOAD);
	}
	else
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			Texture->VramClut/256, Texture->ClutPSM, 0, 0, GS_CLUT_STOREMODE_LOAD);
	}
	*p_data++ = GS_TEX0_1+gsGlobal->PrimContext;

	*p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_TRIFAN, 0, 1, gsGlobal->PrimFogEnable,
				gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
				1, gsGlobal->PrimContext, 0);

	*p_data++ = GS_PRIM;

	*p_data++ = color;
	*p_data++ = GS_RGBAQ;

	for(count = 0; count < (verticies * 5); count+=5)
	{
		*p_data++ = GS_SETREG_UV( vertexdata[count+3], vertexdata[count+4] );
		*p_data++ = GS_UV;

		*p_data++ = GS_SETREG_XYZ2( vertexdata[count], vertexdata[count+1], vertexdata[count+2] );
		*p_data++ = GS_XYZ2;
	}
}

void gsKit_prim_quad_texture_3d(GSGLOBAL *gsGlobal, GSTEXTURE *Texture,
				float x1, float y1, int iz1, float u1, float v1,
				float x2, float y2, int iz2, float u2, float v2,
				float x3, float y3, int iz3, float u3, float v3,
				float x4, float y4, int iz4, float u4, float v4, u64 color)
{
	gsKit_set_texfilter(gsGlobal, Texture->Filter);

	u64* p_store;
	u64* p_data;
	int qsize = 6;
	int bsize = 96;

	int tw, th;
	gsKit_set_tw_th(Texture, &tw, &th);

	int ix1 = gsKit_float_to_int_x(gsGlobal, x1);
	int ix2 = gsKit_float_to_int_x(gsGlobal, x2);
	int ix3 = gsKit_float_to_int_x(gsGlobal, x3);
	int ix4 = gsKit_float_to_int_x(gsGlobal, x4);
	int iy1 = gsKit_float_to_int_y(gsGlobal, y1);
	int iy2 = gsKit_float_to_int_y(gsGlobal, y2);
	int iy3 = gsKit_float_to_int_y(gsGlobal, y3);
	int iy4 = gsKit_float_to_int_y(gsGlobal, y4);

	int iu1 = gsKit_float_to_int_u(Texture, u1);
	int iu2 = gsKit_float_to_int_u(Texture, u2);
	int iu3 = gsKit_float_to_int_u(Texture, u3);
	int iu4 = gsKit_float_to_int_u(Texture, u4);
	int iv1 = gsKit_float_to_int_v(Texture, v1);
	int iv2 = gsKit_float_to_int_v(Texture, v2);
	int iv3 = gsKit_float_to_int_v(Texture, v3);
	int iv4 = gsKit_float_to_int_v(Texture, v4);

	p_store = p_data = gsKit_heap_alloc(gsGlobal, qsize, bsize, GSKIT_GIF_PRIM_QUAD_TEXTURED);

	*p_data++ = GIF_TAG_QUAD_TEXTURED(0);
	*p_data++ = GIF_TAG_QUAD_TEXTURED_REGS(gsGlobal->PrimContext);

	if(Texture->VramClut == 0)
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			0, 0, 0, 0, GS_CLUT_STOREMODE_NOLOAD);
	}
	else
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			Texture->VramClut/256, Texture->ClutPSM, 0, 0, GS_CLUT_STOREMODE_LOAD);
	}

	*p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_TRISTRIP, 0, 1, gsGlobal->PrimFogEnable,
				gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
				1, gsGlobal->PrimContext, 0);

	*p_data++ = color;

	*p_data++ = GS_SETREG_UV( iu1, iv1 );
	*p_data++ = GS_SETREG_XYZ2( ix1, iy1, iz1 );

	*p_data++ = GS_SETREG_UV( iu2, iv2 );
	*p_data++ = GS_SETREG_XYZ2( ix2, iy2, iz2 );

	*p_data++ = GS_SETREG_UV( iu3, iv3 );
	*p_data++ = GS_SETREG_XYZ2( ix3, iy3, iz3 );

	*p_data++ = GS_SETREG_UV( iu4, iv4 );
	*p_data++ = GS_SETREG_XYZ2( ix4, iy4, iz4 );
}

void gsKit_prim_quad_goraud_texture_3d(GSGLOBAL *gsGlobal, GSTEXTURE *Texture,
				float x1, float y1, int iz1, float u1, float v1,
				float x2, float y2, int iz2, float u2, float v2,
				float x3, float y3, int iz3, float u3, float v3,
				float x4, float y4, int iz4, float u4, float v4,
				u64 color1, u64 color2, u64 color3, u64 color4)
{
	gsKit_set_texfilter(gsGlobal, Texture->Filter);

	u64* p_store;
	u64* p_data;
	int qsize = 7;
	int bsize = 112;

	int tw, th;
	gsKit_set_tw_th(Texture, &tw, &th);

	int ix1 = gsKit_float_to_int_x(gsGlobal, x1);
	int ix2 = gsKit_float_to_int_x(gsGlobal, x2);
	int ix3 = gsKit_float_to_int_x(gsGlobal, x3);
	int ix4 = gsKit_float_to_int_x(gsGlobal, x4);
	int iy1 = gsKit_float_to_int_y(gsGlobal, y1);
	int iy2 = gsKit_float_to_int_y(gsGlobal, y2);
	int iy3 = gsKit_float_to_int_y(gsGlobal, y3);
	int iy4 = gsKit_float_to_int_y(gsGlobal, y4);

	int iu1 = gsKit_float_to_int_u(Texture, u1);
	int iu2 = gsKit_float_to_int_u(Texture, u2);
	int iu3 = gsKit_float_to_int_u(Texture, u3);
	int iu4 = gsKit_float_to_int_u(Texture, u4);
	int iv1 = gsKit_float_to_int_v(Texture, v1);
	int iv2 = gsKit_float_to_int_v(Texture, v2);
	int iv3 = gsKit_float_to_int_v(Texture, v3);
	int iv4 = gsKit_float_to_int_v(Texture, v4);

	p_store = p_data = gsKit_heap_alloc(gsGlobal, qsize, bsize, GSKIT_GIF_PRIM_QUAD_TEXTURED);

	*p_data++ = GIF_TAG_QUAD_GORAUD_TEXTURED(0);
	*p_data++ = GIF_TAG_QUAD_GORAUD_TEXTURED_REGS(gsGlobal->PrimContext);

	if(Texture->VramClut == 0)
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			0, 0, 0, 0, GS_CLUT_STOREMODE_NOLOAD);
	}
	else
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			Texture->VramClut/256, Texture->ClutPSM, 0, 0, GS_CLUT_STOREMODE_LOAD);
	}

	*p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_TRISTRIP, 1, 1, gsGlobal->PrimFogEnable,
				gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
				1, gsGlobal->PrimContext, 0);

	*p_data++ = color1;
	*p_data++ = GS_SETREG_UV( iu1, iv1 );
	*p_data++ = GS_SETREG_XYZ2( ix1, iy1, iz1 );

	*p_data++ = color2;
	*p_data++ = GS_SETREG_UV( iu2, iv2 );
	*p_data++ = GS_SETREG_XYZ2( ix2, iy2, iz2 );

	*p_data++ = color3;
	*p_data++ = GS_SETREG_UV( iu3, iv3 );
	*p_data++ = GS_SETREG_XYZ2( ix3, iy3, iz3 );

	*p_data++ = color4;
	*p_data++ = GS_SETREG_UV( iu4, iv4 );
	*p_data++ = GS_SETREG_XYZ2( ix4, iy4, iz4 );
}
