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

#include "gsKit.h"

#ifdef HAVE_LIBJPG
#include <libjpg.h>
#endif

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
	if(psm == GS_PSM_T8 || psm == GS_PSM_T4)

		width = (-GS_VRAM_TBWALIGN_CLUT)&(width+GS_VRAM_TBWALIGN_CLUT-1);
	else
		width = (-GS_VRAM_TBWALIGN)&(width+GS_VRAM_TBWALIGN-1);

	height = (-GS_VRAM_TBWALIGN)&(height+GS_VRAM_TBWALIGN-1);

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

int gsKit_texture_png(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, char *Path)
{
	printf("ERROR: gsKit_texture_png unimplimented.\n");
	return -1;
}

int gsKit_texture_bmp(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, char *Path)
{
	GSBITMAP Bitmap;
	int x, y;
	int cy;
	u32 FTexSize;
	u8  *image;
	u8  *p;

	int File = fioOpen(Path, O_RDONLY);
	if (fioRead(File, &Bitmap.FileHeader, sizeof(Bitmap.FileHeader)) <= 0)
	{
		printf("Could not load bitmap: %s\n", Path);
		return -1;
	}
		
	if (fioRead(File, &Bitmap.InfoHeader, sizeof(Bitmap.InfoHeader)) <= 0)
	{
		printf("Could not load bitmap: %s\n", Path);
		return -1;
	}

	Texture->Width = Bitmap.InfoHeader.Width;
	Texture->Height = Bitmap.InfoHeader.Height;
	Texture->Filter = GS_FILTER_NEAREST;

	if(Bitmap.InfoHeader.BitCount == 4)
	{
		Texture->PSM = GS_PSM_T4;
		printf("NOTICE: GS_PSM_T4 Is Currently Unsupported\n");
	}
	else if(Bitmap.InfoHeader.BitCount == 8)
    {
		Texture->PSM = GS_PSM_T8;
		Texture->Clut = memalign(128, gsKit_texture_size_ee(16, 16, GS_PSM_CT32));
		Texture->ClutPSM = GS_PSM_CT32;

		memset(Texture->Clut, 0, gsKit_texture_size_ee(16, 16, GS_PSM_CT32));
		Texture->VramClut = gsKit_vram_alloc(gsGlobal, gsKit_texture_size(16, 16, GS_PSM_CT32), GSKIT_ALLOC_USERBUFFER);
		fioLseek(File, 54, SEEK_SET);
		if(fioRead(File, Texture->Clut, Bitmap.InfoHeader.ColorUsed*sizeof(long)) <= 0)
		{
			printf("Could not load bitmap: %s\n", Path);
			return -1;
		}

		GSBMCLUT *clut = (GSBMCLUT *)Texture->Clut;
		int i;
		for (i = Bitmap.InfoHeader.ColorUsed; i < 256; i++)
		{
			memset(&clut[i], 0, sizeof(clut[i]));
		}

		for (i = 0; i < 256; i++)
		{
			u8 tmp = clut[i].Blue;
			clut[i].Blue = clut[i].Red;
			clut[i].Red = tmp;
			clut[i].Alpha = 0;
		}

		// rotate clut
		for (i = 0; i < 256; i++)
		{
			if ((i&0x18) == 8)
			{
				GSBMCLUT tmp = clut[i];
				clut[i] = clut[i+8];
				clut[i+8] = tmp;
			}
		}
	}
	else if(Bitmap.InfoHeader.BitCount == 16)
    {
		Texture->PSM = GS_PSM_CT16;
		Texture->VramClut = 0;
		Texture->Clut = NULL;
	}
	else if(Bitmap.InfoHeader.BitCount == 24)
	{
		Texture->PSM = GS_PSM_CT24;
		Texture->VramClut = 0;
		Texture->Clut = NULL;
	}

	FTexSize = fioLseek(File, 0, SEEK_END);
	FTexSize -= Bitmap.FileHeader.Offset;

	fioLseek(File, Bitmap.FileHeader.Offset, SEEK_SET);

	u32 TextureSize = gsKit_texture_size_ee(Texture->Width, Texture->Height, Texture->PSM);
	u32 VramTextureSize = gsKit_texture_size(Texture->Width, Texture->Height, Texture->PSM);

	Texture->Mem = memalign(128,TextureSize);

	// Shouldn't we just always be doing this?
	if(Bitmap.InfoHeader.BitCount == 24)
	{
		image = memalign(128, FTexSize);
		if (image == NULL) return -1;
		fioRead(File, image, FTexSize);
		p = Texture->Mem;
		for (y=Texture->Height-1,cy=0; y>=0; y--,cy++) {
			for (x=0; x<Texture->Width; x++) {
				p[(y*Texture->Width+x)*3+2] = image[(cy*Texture->Width+x)*3+0];
				p[(y*Texture->Width+x)*3+1] = image[(cy*Texture->Width+x)*3+1];
				p[(y*Texture->Width+x)*3+0] = image[(cy*Texture->Width+x)*3+2];
			}
		}
		free(image);
	}
	else if(Bitmap.InfoHeader.BitCount == 16)
	{
		printf("16-bit BMP not supported yet.\n");
	}
	else if(Bitmap.InfoHeader.BitCount == 8)
	{
		//fioRead(File, Texture->Mem, FTexSize);
		char *tex = Texture->Mem;
		image = memalign(128,FTexSize);
		fioRead(File, image, FTexSize);
		for (y=Texture->Height-1; y>=0; y--) 
		{
			memcpy(&tex[y*Texture->Width], &image[(Texture->Height-y-1)*Texture->Width], Texture->Width);
		}
		free(image);
	}
	else if(Bitmap.InfoHeader.BitCount == 4)
	{
		printf("16-bit BMP not supported yet.\n");
	}
	else
	{
		printf("Unknown BMP bit depth format %d\n", Bitmap.InfoHeader.BitCount);
	}

	fioClose(File);

	Texture->Vram = gsKit_vram_alloc(gsGlobal, VramTextureSize, GSKIT_ALLOC_USERBUFFER);
	if(Texture->Vram == GSKIT_ALLOC_ERROR)
	{
		printf("VRAM Allocation Failed. Will not upload texture.\n");
		return -1;
	}

	gsKit_texture_upload(gsGlobal, Texture);
	free(Texture->Mem);

        return 0;
}

int  gsKit_texture_jpeg(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, char *Path)
{
#ifdef HAVE_LIBJPG
	
	jpgData *jpg;

	int TextureSize = 0;
	int VramTextureSize = 0;

	jpg = jpgOpen(Path);
	if (jpg == NULL) {
		return -1;
		printf("error opening %s\n", Path);
	}
	
	Texture->Width =  jpg->width;
	Texture->Height = jpg->height;
	Texture->PSM = GS_PSM_CT24;
	Texture->Filter = GS_FILTER_NEAREST;
	Texture->VramClut = 0;
	Texture->Clut = NULL;

	TextureSize = gsKit_texture_size_ee(Texture->Width, Texture->Height, Texture->PSM);
	VramTextureSize = gsKit_texture_size(Texture->Width, Texture->Height, Texture->PSM);
	#ifdef DEBUG
	printf("Texture Size = %i\n",TextureSize);
	#endif

	Texture->Mem = memalign(128,TextureSize);
	jpgReadImage(jpg, Texture->Mem);
	jpgClose(jpg);
	
	Texture->Vram = gsKit_vram_alloc(gsGlobal, VramTextureSize, GSKIT_ALLOC_USERBUFFER);
	if(Texture->Vram == GSKIT_ALLOC_ERROR)
	{
		printf("VRAM Allocation Failed. Will not upload texture.\n");
		return -1;
	}
	gsKit_texture_upload(gsGlobal, Texture);
	
	free(Texture->Mem);	
	
	return 0;
	
#else

	printf("ERROR: gsKit_texture_jpeg unimplimented.\n");
	return -1;
	
#endif
}

int gsKit_texture_tga(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, char *Path)
{
	printf("ERROR: gsKit_texture_tga unimplimented.\n");
	return -1;
}

int gsKit_texture_raw(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, char *Path)
{
	int File = fioOpen(Path, O_RDONLY);
	int FileSize = gsKit_texture_size_ee(Texture->Width, Texture->Height, Texture->PSM);
	int VramFileSize = gsKit_texture_size(Texture->Width, Texture->Height, Texture->PSM);
	Texture->Mem = memalign(128, FileSize);
	Texture->Vram = gsKit_vram_alloc(gsGlobal, VramFileSize, GSKIT_ALLOC_USERBUFFER);

	if(Texture->PSM != GS_PSM_T8 && Texture->PSM != GS_PSM_T4)
	{
		Texture->VramClut = 0;
		Texture->Clut = NULL;
	}

	if(Texture->Vram == GSKIT_ALLOC_ERROR)
	{
		printf("VRAM Allocation Failed. Will not upload texture.\n");
		return -1;
	}
	
        if(fioRead(File, Texture->Mem, FileSize) <= 0)
        {
                printf("Could not load texture: %s\n", Path);
                return -1;
        }
	fioClose(File);
	gsKit_texture_upload(gsGlobal, Texture);
	free(Texture->Mem);
	return 0;
}

int gsKit_texture_fnt_raw(GSGLOBAL *gsGlobal, GSFONT *gsFont)
{
	u32 *data = (u32*)gsFont->RawData;
	u32 *mem;
	int size = 0;
	int vramsize = 0;
	int i;

	gsFont->Texture->Width  = data[1];
	gsFont->Texture->Height = data[2];
	gsFont->Texture->PSM    = data[3];
	gsFont->Texture->Filter = GS_FILTER_NEAREST;
	if(gsFont->Texture->PSM != GS_PSM_T8 && gsFont->Texture->PSM != GS_PSM_T4)
	{
		gsFont->Texture->VramClut = 0;
		gsFont->Texture->Clut = NULL;
	}

	gsFont->HChars          = data[4];
	gsFont->VChars          = data[5];
	gsFont->CharWidth       = data[6];
	gsFont->CharHeight      = data[7];

	size = gsKit_texture_size_ee(gsFont->Texture->Width, gsFont->Texture->Height, gsFont->Texture->PSM);
	vramsize = gsKit_texture_size(gsFont->Texture->Width, gsFont->Texture->Height, gsFont->Texture->PSM);
	gsFont->Texture->Mem = memalign(128, size);
	gsFont->Texture->Vram = gsKit_vram_alloc(gsGlobal, vramsize, GSKIT_ALLOC_USERBUFFER);
	if(gsFont->Texture->Vram == GSKIT_ALLOC_ERROR)
	{
		printf("VRAM Allocation Failed. Will not upload texture.\n");
		return -1;
	}
	
	memcpy(gsFont->Texture->Mem, &data[288/4], size);

	if (gsFont->Texture->PSM != GS_PSM_CT32) {
		printf("Unsupported fnt PSM %d\n", gsFont->Texture->PSM);
	}
	mem = (u32*)gsFont->Texture->Mem;
	for (i=0; i<size/4; i++) {
		if (mem[i] == 0xFF00FFFF) {
			mem[i] = 0;
		} else {
			u32 c = (mem[i] & 0x00FF0000) >> 16;
			mem[i] = 0x80000000 | (c) | (c<<8) | (c<<16);
		}
	}

	gsKit_texture_upload(gsGlobal, gsFont->Texture);
	free(gsFont->Texture->Mem);
	return 0;
}


int gsKit_texture_fnt(GSGLOBAL *gsGlobal, GSFONT *gsFont)
{
	u32 *mem;
	int File;
	int size = 0;
	int vramsize = 0;
	int i;

	File = fioOpen(gsFont->Path, O_RDONLY);
	fioLseek(File, 4, SEEK_SET);
        if(fioRead(File, &gsFont->Texture->Width, 4) <= 0)
        {
                printf("Could not load font: %s\n", gsFont->Path);
                return -1;
        }
	if(fioRead(File, &gsFont->Texture->Height, 4) <= 0)
        {
                printf("Could not load font: %s\n", gsFont->Path);
                return -1;
        }
	if(fioRead(File, &gsFont->Texture->PSM, 4) <= 0)
        {
                printf("Could not load font: %s\n", gsFont->Path);
                return -1;
        }
        if(fioRead(File, &gsFont->HChars, 4) <= 0)
        {
                printf("Could not load font: %s\n", gsFont->Path);
                return -1;
        }
        if(fioRead(File, &gsFont->VChars, 4) <= 0)
        {
                printf("Could not load font: %s\n", gsFont->Path);
                return -1;
        }
        if(fioRead(File, &gsFont->CharWidth, 4) <= 0)
        {
                printf("Could not load font: %s\n", gsFont->Path);
                return -1;
        }
        if(fioRead(File, &gsFont->CharHeight, 4) <= 0)
        {
                printf("Could not load font: %s\n", gsFont->Path);
                return -1;
        }
	fioLseek(File, 288, SEEK_SET);

	gsFont->Texture->Filter = GS_FILTER_NEAREST;

	size = gsKit_texture_size_ee(gsFont->Texture->Width, gsFont->Texture->Height, gsFont->Texture->PSM);
	vramsize = gsKit_texture_size(gsFont->Texture->Width, gsFont->Texture->Height, gsFont->Texture->PSM);
	gsFont->Texture->Mem = memalign(128, size);
	gsFont->Texture->Vram = gsKit_vram_alloc(gsGlobal, vramsize, GSKIT_ALLOC_USERBUFFER);
	if(gsFont->Texture->Vram == GSKIT_ALLOC_ERROR)
	{
		printf("VRAM Allocation Failed. Will not upload texture.\n");
		return -1;
	}
	
	if(fioRead(File, gsFont->Texture->Mem, size) <= 0)
	{
		printf("Could not load font: %s\n", gsFont->Path);
		return -1;
	}
	fioClose(File);

	if (gsFont->Texture->PSM != 0) {
		printf("Unsupported fnt PSM %d\n", gsFont->Texture->PSM);
	}
	mem = (u32*)gsFont->Texture->Mem;
	for (i=0; i<size/4; i++) {
		if (mem[i] == 0xFF00FFFF) {
			mem[i] = 0;
		} else {
			u32 c = (mem[i] & 0x00FF0000) >> 16;
			mem[i] = 0x80000000 | (c) | (c<<8) | (c<<16);
		}
	}

	gsKit_texture_upload(gsGlobal, gsFont->Texture);
	free(gsFont->Texture->Mem);
	return 0;
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
#ifdef DEBUG
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

//	p_store = p_data = dmaKit_spr_alloc( p_size*16 );
	p_store = p_data = memalign(64, (p_size * 16));

	FlushCache(0);

	// DMA DATA
	*p_data++ = DMA_TAG( 5, 0, DMA_CNT, 0, 0, 0 );
	*p_data++ = 0;

	*p_data++ = GIF_TAG( 4, 0, 0, 0, 0, 1 );
	*p_data++ = GIF_AD;
	
	if(tbp % 256)
		printf("ERROR: Texture base pointer not on 256 byte alignment - Modulo = %d\n", tbp % 256);

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

		*p_data++ = GS_TEXFLUSH;
		*p_data++ = 0;
	}
	
//        if(gsGlobal->DrawMode == GS_IMMEDIATE)
//        {
                dmaKit_send_chain( DMA_CHANNEL_GIF, 0, p_store, p_size);
                dmaKit_wait_fast( DMA_CHANNEL_GIF );
		free(p_store);
//        }
//        else
//                gsKit_queue_add( gsGlobal, DMA_CHANNEL_GIF, p_store, size );

}

void gsKit_texture_upload(GSGLOBAL *gsGlobal, GSTEXTURE *Texture)
{
	if(Texture->PSM == GS_PSM_T8 || Texture->PSM == GS_PSM_T4)
	{
		Texture->TBW = (-GS_VRAM_TBWALIGN_CLUT)&(Texture->Width+GS_VRAM_TBWALIGN_CLUT-1);
		if(Texture->TBW / 64 > 0)
			Texture->TBW = (Texture->TBW / 64);
		else
			Texture->TBW = 1;
	}
	else
	{
		Texture->TBW = (-GS_VRAM_TBWALIGN)&(Texture->Width+GS_VRAM_TBWALIGN-1);
		if(Texture->TBW / 64 > 0)
			Texture->TBW = (Texture->TBW / 64);
		else
			Texture->TBW = 1;
	}

	if (Texture->PSM == GS_PSM_T8)
	{
		gsKit_texture_send(Texture->Mem, Texture->Width, Texture->Height, Texture->Vram, Texture->PSM, Texture->TBW, GS_CLUT_TEXTURE);
  		if(Texture->ClutPSM == GS_PSM_CT32)
			gsKit_texture_send(Texture->Clut, 16, 16, Texture->VramClut, Texture->ClutPSM, 1, GS_CLUT_PALLETE);
		else
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


void gsKit_prim_sprite_texture_3d(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, 	
				float x1, float y1, int iz1, float u1, float v1,
				float x2, float y2, int iz2, float u2, float v2, u64 color)
{
        u64* p_store;
        u64* p_data;
        int qsize = 8;
        int bsize = 128;

        if(gsGlobal->Field == GS_FRAME)
        {
		y1 /= 2;
		y2 /= 2;
#ifdef GSKIT_ENABLE_HBOFFSET
                if(!gsGlobal->EvenOrOdd)
                {
                        y1 += 0.5f;
                        y2 += 0.5f;
                }
#endif
	}

	int ix1 = (int)(x1 * 16.0f) + gsGlobal->Offset;
	int ix2 = (int)(x2 * 16.0f) + gsGlobal->Offset;
	int iy1 = (int)(y1 * 16.0f) + gsGlobal->Offset;
	int iy2 = (int)(y2 * 16.0f) + gsGlobal->Offset;

	int iu1 = (int)(u1 * 16.0f);
	int iu2 = (int)(u2 * 16.0f);
	int iv1 = (int)(v1 * 16.0f);
	int iv2 = (int)(v2 * 16.0f);

	
	int tw = 31 - (lzw(Texture->Width) + 1);
	if(Texture->Width > (1<<tw))
		tw++;

	int th = 31 - (lzw(Texture->Height) + 1);
	if(Texture->Height > (1<<th))
		th++;

        if( gsGlobal->PrimAlphaEnable == 1 )
	{
                qsize++;
		bsize += 16;
	}

        p_store = p_data = gsKit_spr_alloc(gsGlobal, qsize, bsize);

	if(Texture->VramClut == 0)
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			0, 0, Texture->ClutPSM, 0, GS_CLUT_STOREMODE_NOLOAD);
	}
	else
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			Texture->VramClut/256, 0, 0, 0, GS_CLUT_STOREMODE_LOAD);
	}
        *p_data++ = GS_TEX0_1+gsGlobal->PrimContext;

	*p_data++ = GS_SETREG_TEX1(0, 0, Texture->Filter, Texture->Filter, 0, 0, 0);
	*p_data++ = GS_TEX1_1+gsGlobal->PrimContext;

        if( gsGlobal->PrimAlphaEnable == 1 )
        {
                *p_data++ = gsGlobal->PrimAlpha;
                *p_data++ = GS_ALPHA_1+gsGlobal->PrimContext;
        }
 
        *p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_SPRITE, 0, 1, gsGlobal->PrimFogEnable,
                                    gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
                                    1, gsGlobal->PrimContext, 0);
        
        *p_data++ = GS_PRIM;
        
        *p_data++ = color;
        *p_data++ = GS_RGBAQ;

        *p_data++ = GS_SETREG_UV( iu1, iv1 );
        *p_data++ = GS_UV;

        *p_data++ = GS_SETREG_XYZ2( ix1, iy1, iz1 );
        *p_data++ = GS_XYZ2;

        *p_data++ = GS_SETREG_UV( iu2, iv2 );
        *p_data++ = GS_UV;

        *p_data++ = GS_SETREG_XYZ2( ix2, iy2, iz2 );
        *p_data++ = GS_XYZ2;
}

void gsKit_prim_triangle_texture_3d(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, 	
				float x1, float y1, int iz1, float u1, float v1,
				float x2, float y2, int iz2, float u2, float v2,
				float x3, float y3, int iz3, float u3, float v3, u64 color)
{
        u64* p_store;
        u64* p_data;
        int qsize = 10;
        int bsize = 160;

	int tw = 31 - (lzw(Texture->Width) + 1);
	if(Texture->Width > (1<<tw))
		tw++;

	int th = 31 - (lzw(Texture->Height) + 1);
	if(Texture->Height > (1<<th))
		th++;

        if(gsGlobal->Field == GS_FRAME)
        {
                y1 /= 2;
                y2 /= 2;
                y3 /= 2;
#ifdef GSKIT_ENABLE_HBOFFSET
                if(!gsGlobal->EvenOrOdd)
                {
                        y1 += 0.5f;
                        y2 += 0.5f;
                        y3 += 0.5f;
                }
#endif
        }

        int ix1 = (int)(x1 * 16.0f) + gsGlobal->Offset;
        int ix2 = (int)(x2 * 16.0f) + gsGlobal->Offset;
        int ix3 = (int)(x3 * 16.0f) + gsGlobal->Offset;
        int iy1 = (int)(y1 * 16.0f) + gsGlobal->Offset;
        int iy2 = (int)(y2 * 16.0f) + gsGlobal->Offset;
        int iy3 = (int)(y3 * 16.0f) + gsGlobal->Offset;

        int iu1 = (int)(u1 * 16.0f);
        int iu2 = (int)(u2 * 16.0f);
        int iu3 = (int)(u3 * 16.0f);
        int iv1 = (int)(v1 * 16.0f);
        int iv2 = (int)(v2 * 16.0f);
        int iv3 = (int)(v3 * 16.0f);

        if( gsGlobal->PrimAlphaEnable == 1 )
                qsize++;

        p_store = p_data = gsKit_spr_alloc( gsGlobal, qsize, bsize );

	if(Texture->VramClut == 0)
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			0, 0, Texture->ClutPSM, 0, GS_CLUT_STOREMODE_NOLOAD);
	}
	else
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			Texture->VramClut/256, 0, 0, 0, GS_CLUT_STOREMODE_LOAD);
	}
        *p_data++ = GS_TEX0_1+gsGlobal->PrimContext;

	*p_data++ = GS_SETREG_TEX1(0, 0, Texture->Filter, Texture->Filter, 0, 0, 0);
	*p_data++ = GS_TEX1_1+gsGlobal->PrimContext;

        if( gsGlobal->PrimAlphaEnable == 1 )
        {
                *p_data++ = gsGlobal->PrimAlpha;
                *p_data++ = GS_ALPHA_1+gsGlobal->PrimContext;
        }
 
        *p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_TRIANGLE, 0, 1, gsGlobal->PrimFogEnable,
                                    gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
                                    1, gsGlobal->PrimContext, 0);
        
        *p_data++ = GS_PRIM;
        
        *p_data++ = color;
        *p_data++ = GS_RGBAQ;
        
        *p_data++ = GS_SETREG_UV( iu1, iv1 );
        *p_data++ = GS_UV;

        *p_data++ = GS_SETREG_XYZ2( ix1, iy1, iz1 );
        *p_data++ = GS_XYZ2;

        *p_data++ = GS_SETREG_UV( iu2, iv2 );
        *p_data++ = GS_UV;

        *p_data++ = GS_SETREG_XYZ2( ix2, iy2, iz2 );
        *p_data++ = GS_XYZ2;
	
	*p_data++ = GS_SETREG_UV( iu3, iv3 );
        *p_data++ = GS_UV;

        *p_data++ = GS_SETREG_XYZ2( ix3, iy3, iz3 );
        *p_data++ = GS_XYZ2;

}

void gsKit_prim_triangle_strip_texture(GSGLOBAL *gsGlobal, GSTEXTURE *Texture,
					float *TriStrip, int segments, int iz, u64 color)
{
        u64* p_store;
        u64* p_data;
        int qsize = 4 + (segments * 2);
        int count;
        int vertexdata[segments*4];

	int tw = 31 - (lzw(Texture->Width) + 1);
	if(Texture->Width > (1<<tw))
		tw++;

	int th = 31 - (lzw(Texture->Height) + 1);
	if(Texture->Height > (1<<th))
		th++;
 
        for(count = 0; count < (segments * 4); count+=4)
        {
                vertexdata[count] =  (int)((*TriStrip++) * 16.0f) + gsGlobal->Offset;
		if(gsGlobal->Field == GS_FRAME)
                {
                        *(TriStrip) /= 2;
                #ifdef GSKIT_ENABLE_HBOFFSET
                        if(!gsGlobal->EvenOrOdd)
                        {
                        *(TriStrip) += 0.5f;
                        }
                #endif
                }
                vertexdata[count+1] =  (int)((*TriStrip++) * 16.0f) + gsGlobal->Offset;
                vertexdata[count+2] =  (int)((*TriStrip++) * 16.0f);
                vertexdata[count+3] =  (int)((*TriStrip++) * 16.0f);
        }
	
        if( gsGlobal->PrimAlphaEnable == 1 )
                qsize++;

        p_store = p_data = gsKit_spr_alloc( gsGlobal, qsize, (qsize*16) );

	if(Texture->VramClut == 0)
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			0, 0, Texture->ClutPSM, 0, GS_CLUT_STOREMODE_NOLOAD);
	}
	else
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			Texture->VramClut/256, 0, 0, 0, GS_CLUT_STOREMODE_LOAD);
	}
        *p_data++ = GS_TEX0_1+gsGlobal->PrimContext;

	*p_data++ = GS_SETREG_TEX1(0, 0, Texture->Filter, Texture->Filter, 0, 0, 0);
	*p_data++ = GS_TEX1_1+gsGlobal->PrimContext;

        if( gsGlobal->PrimAlphaEnable == 1 )
        {
                *p_data++ = gsGlobal->PrimAlpha;
                *p_data++ = GS_ALPHA_1+gsGlobal->PrimContext;
        }
 
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
        u64* p_store;
        u64* p_data;
        int qsize = 4 + (segments * 2);
        int count;
        int vertexdata[segments*5];

	int tw = 31 - (lzw(Texture->Width) + 1);
	if(Texture->Width > (1<<tw))
		tw++;

	int th = 31 - (lzw(Texture->Height) + 1);
	if(Texture->Height > (1<<th))
		th++;
 
        for(count = 0; count < (segments * 5); count+=5)
        {
                vertexdata[count] = (int)((*TriStrip++) * 16.0f) + gsGlobal->Offset;
                if(gsGlobal->Field == GS_FRAME)
                {
                        *(TriStrip) /= 2;
                #ifdef GSKIT_ENABLE_HBOFFSET
                        if(!gsGlobal->EvenOrOdd)
                        {
	                        *(TriStrip) += 0.5f;
                        }
                #endif
                }
                vertexdata[count+1] = (int)((*TriStrip++) * 16.0f) + gsGlobal->Offset;
                vertexdata[count+2] = (int)((*TriStrip++) * 16.0f);
                vertexdata[count+3] = (int)((*TriStrip++) * 16.0f);
                vertexdata[count+4] = (int)((*TriStrip++) * 16.0f);
        }
	
        if( gsGlobal->PrimAlphaEnable == 1 )
		qsize++;

        p_store = p_data = gsKit_spr_alloc( gsGlobal, qsize, (qsize*16) );

	if(Texture->VramClut == 0)
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			0, 0, Texture->ClutPSM, 0, GS_CLUT_STOREMODE_NOLOAD);
	}
	else
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			Texture->VramClut/256, 0, 0, 0, GS_CLUT_STOREMODE_LOAD);
	}
        *p_data++ = GS_TEX0_1+gsGlobal->PrimContext;

	*p_data++ = GS_SETREG_TEX1(0, 0, Texture->Filter, Texture->Filter, 0, 0, 0);
	*p_data++ = GS_TEX1_1+gsGlobal->PrimContext;

        if( gsGlobal->PrimAlphaEnable == 1 )
        {
                *p_data++ = gsGlobal->PrimAlpha;
                *p_data++ = GS_ALPHA_1+gsGlobal->PrimContext;
        }
 
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
        u64* p_store;
        u64* p_data;
        int qsize = 4 + (verticies * 2);
        int count;
        int vertexdata[verticies*4];

	int tw = 31 - (lzw(Texture->Width) + 1);
	if(Texture->Width > (1<<tw))
		tw++;

	int th = 31 - (lzw(Texture->Height) + 1);
	if(Texture->Height > (1<<th))
		th++;
 
        for(count = 0; count < (verticies * 4); count+=4)
        {
                vertexdata[count] =  (int)((*TriFan++) * 16.0f) + gsGlobal->Offset;
                if(gsGlobal->Field == GS_FRAME)
                {
                        *(TriFan) /= 2;
                #ifdef GSKIT_ENABLE_HBOFFSET
                        if(!gsGlobal->EvenOrOdd)
                        {
	                        *(TriFan) += 0.5f;
                        }
                #endif
                }
                vertexdata[count+1] =  (int)((*TriFan++) * 16.0f) + gsGlobal->Offset;
                vertexdata[count+2] =  (int)((*TriFan++) * 16.0f);
                vertexdata[count+3] =  (int)((*TriFan++) * 16.0f);
        }
	
        if( gsGlobal->PrimAlphaEnable == 1 )
                qsize++;

        p_store = p_data = gsKit_spr_alloc( gsGlobal, qsize, (qsize*16) );

	if(Texture->VramClut == 0)
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			0, 0, Texture->ClutPSM, 0, GS_CLUT_STOREMODE_NOLOAD);
	}
	else
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			Texture->VramClut/256, 0, 0, 0, GS_CLUT_STOREMODE_LOAD);
	}
        *p_data++ = GS_TEX0_1+gsGlobal->PrimContext;

	*p_data++ = GS_SETREG_TEX1(0, 0, Texture->Filter, Texture->Filter, 0, 0, 0);
	*p_data++ = GS_TEX1_1+gsGlobal->PrimContext;

        if( gsGlobal->PrimAlphaEnable == 1 )
        {
                *p_data++ = gsGlobal->PrimAlpha;
                *p_data++ = GS_ALPHA_1+gsGlobal->PrimContext;
        }
 
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
        u64* p_store;
        u64* p_data;
        int qsize = 4 + (verticies * 2);
        int count;
        int vertexdata[verticies*5];

	int tw = 31 - (lzw(Texture->Width) + 1);
	if(Texture->Width > (1<<tw))
		tw++;

	int th = 31 - (lzw(Texture->Height) + 1);
	if(Texture->Height > (1<<th))
		th++;
 
        for(count = 0; count < (verticies * 5); count+=5)
        {
                vertexdata[count] =  (int)((*TriFan++) * 16.0f) + gsGlobal->Offset;
                if(gsGlobal->Field == GS_FRAME)
                {
                        *(TriFan) /= 2;
                #ifdef GSKIT_ENABLE_HBOFFSET
                        if(!gsGlobal->EvenOrOdd)
                        {
	                        *(TriFan) += 0.5f;
                        }
                #endif
                }
                vertexdata[count+1] =  (int)((*TriFan++) * 16.0f) + gsGlobal->Offset;
                vertexdata[count+2] =  (int)((*TriFan++) * 16.0f);
                vertexdata[count+3] =  (int)((*TriFan++) * 16.0f);
                vertexdata[count+4] =  (int)((*TriFan++) * 16.0f);
        }
	
        if( gsGlobal->PrimAlphaEnable == 1 )
		qsize++;

        p_store = p_data = gsKit_spr_alloc( gsGlobal, qsize, (qsize*16) );

	if(Texture->VramClut == 0)
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			0, 0, Texture->ClutPSM, 0, GS_CLUT_STOREMODE_NOLOAD);
	}
	else
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			Texture->VramClut/256, 0, 0, 0, GS_CLUT_STOREMODE_LOAD);
	}
        *p_data++ = GS_TEX0_1+gsGlobal->PrimContext;

	*p_data++ = GS_SETREG_TEX1(0, 0, Texture->Filter, Texture->Filter, 0, 0, 0);
	*p_data++ = GS_TEX1_1+gsGlobal->PrimContext;

        if( gsGlobal->PrimAlphaEnable == 1 )
        {
                *p_data++ = gsGlobal->PrimAlpha;
                *p_data++ = GS_ALPHA_1+gsGlobal->PrimContext;
        }
 
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
        u64* p_store;
        u64* p_data;
        int qsize = 12;
        int bsize = 192;

	int tw = 31 - (lzw(Texture->Width) + 1);
	if(Texture->Width > (1<<tw))
		tw++;

	int th = 31 - (lzw(Texture->Height) + 1);
	if(Texture->Height > (1<<th))
		th++;

        if(gsGlobal->Field == GS_FRAME)
        {
                y1 /= 2;
                y2 /= 2;
                y3 /= 2;
                y4 /= 2;
#ifdef GSKIT_ENABLE_HBOFFSET
                if(!gsGlobal->EvenOrOdd)
                {
                        y1 += 0.5f;
                        y2 += 0.5f;
                        y3 += 0.5f;
                        y4 += 0.5f;
                }
#endif
        }



        int ix1 = (int)(x1 * 16.0f) + gsGlobal->Offset;
        int ix2 = (int)(x2 * 16.0f) + gsGlobal->Offset;
        int ix3 = (int)(x3 * 16.0f) + gsGlobal->Offset;
        int ix4 = (int)(x4 * 16.0f) + gsGlobal->Offset;

        int iy1 = (int)(y1 * 16.0f) + gsGlobal->Offset;
        int iy2 = (int)(y2 * 16.0f) + gsGlobal->Offset;
        int iy3 = (int)(y3 * 16.0f) + gsGlobal->Offset;
        int iy4 = (int)(y4 * 16.0f) + gsGlobal->Offset;

        int iu1 = (int)(u1 * 16.0f);
        int iu2 = (int)(u2 * 16.0f);
        int iu3 = (int)(u3 * 16.0f);
        int iu4 = (int)(u4 * 16.0f);

        int iv1 = (int)(v1 * 16.0f);
        int iv2 = (int)(v2 * 16.0f);
        int iv3 = (int)(v3 * 16.0f);
        int iv4 = (int)(v4 * 16.0f);

        if( gsGlobal->PrimAlphaEnable == 1 )
	{
                qsize++;
		bsize += 16;
	}

        p_store = p_data = gsKit_spr_alloc( gsGlobal, qsize, bsize );

	if(Texture->VramClut == 0)
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			0, 0, Texture->ClutPSM, 0, GS_CLUT_STOREMODE_NOLOAD);
	}
	else
	{
		*p_data++ = GS_SETREG_TEX0(Texture->Vram/256, Texture->TBW, Texture->PSM,
			tw, th, gsGlobal->PrimAlphaEnable, 0,
			Texture->VramClut/256, 0, 0, 0, GS_CLUT_STOREMODE_LOAD);
	}
        *p_data++ = GS_TEX0_1+gsGlobal->PrimContext;

	*p_data++ = GS_SETREG_TEX1(0, 0, Texture->Filter, Texture->Filter, 0, 0, 0);
	*p_data++ = GS_TEX1_1+gsGlobal->PrimContext;

        if( gsGlobal->PrimAlphaEnable == 1 )
        {
                *p_data++ = gsGlobal->PrimAlpha;
                *p_data++ = GS_ALPHA_1+gsGlobal->PrimContext;
        }
 
        *p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_TRISTRIP, 0, 1, gsGlobal->PrimFogEnable,
                                    gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
                                    1, gsGlobal->PrimContext, 0);
        
        *p_data++ = GS_PRIM;
        
        *p_data++ = color;
        *p_data++ = GS_RGBAQ;
        
        *p_data++ = GS_SETREG_UV( iu1, iv1 );
        *p_data++ = GS_UV;

        *p_data++ = GS_SETREG_XYZ2( ix1, iy1, iz1 );
        *p_data++ = GS_XYZ2;

        *p_data++ = GS_SETREG_UV( iu2, iv2 );
        *p_data++ = GS_UV;

        *p_data++ = GS_SETREG_XYZ2( ix2, iy2, iz2 );
        *p_data++ = GS_XYZ2;

	*p_data++ = GS_SETREG_UV( iu3, iv3 );
        *p_data++ = GS_UV;

        *p_data++ = GS_SETREG_XYZ2( ix3, iy3, iz3 );
        *p_data++ = GS_XYZ2;

	*p_data++ = GS_SETREG_UV( iu4, iv4 );
        *p_data++ = GS_UV;

        *p_data++ = GS_SETREG_XYZ2( ix4, iy4, iz4 );
        *p_data++ = GS_XYZ2;
}

