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

#include <stdio.h>
#include <string.h>

#include "gsKit.h"

#ifdef HAVE_LIBJPG
#include <libjpg.h>
#endif

u32  gsKit_texture_size(int width, int height, int psm)
{
	switch (psm) {
		case GS_PSM_CT32:  return (width*height*4);
		case GS_PSM_CT24:  return (width*height*3);
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
	fioLseek(File, 0, SEEK_SET);
        if(fioRead(File, &Bitmap.FileHeader.Type, 2) <= 0)
        {
                printf("Could not load bitmap: %s\n", Path);
                return -1;
        }
	if( Bitmap.FileHeader.Type != 19778 )
	{
		printf("This Bitmap File May Be Invaid. Invalid Type.\n");
		return -1;
	}
	fioLseek(File, 10, SEEK_SET);
        if(fioRead(File, &Bitmap.FileHeader.Offset, 4) <= 0)
        {
                printf("Could not load bitmap: %s\n", Path);
                return -1;
        }

	fioLseek(File, 18, SEEK_SET);
        if(fioRead(File, &Bitmap.InfoHeader.Width, 4) <= 0)
        {
                printf("Could not load bitmap: %s\n", Path);
                return -1;
        }
        if(fioRead(File, &Bitmap.InfoHeader.Height, 4) <= 0)
        {
                printf("Could not load bitmap: %s\n", Path);
                return -1;
        }
	fioLseek(File, 28, SEEK_SET);
        if(fioRead(File, &Bitmap.InfoHeader.PSM, 2) <= 0)
        {
                printf("Could not load bitmap: %s\n", Path);
                return -1;
        }

	Texture->Width = Bitmap.InfoHeader.Width;
	Texture->Height = Bitmap.InfoHeader.Height;

	if(Bitmap.InfoHeader.PSM == 4)
	{
		Texture->PSM = GS_PSM_T4;
		printf("NOTICE: GS_PSM_T4\n");
		// TODO: Do CLUT Stuff.
	}
        else if(Bitmap.InfoHeader.PSM == 8)
        {
                Texture->PSM = GS_PSM_T8;
		printf("NOTICE: GS_PSM_T8\n");
                // TODO: Do CLUT Stuff.
        }
        else if(Bitmap.InfoHeader.PSM == 16)
        {
                Texture->PSM = GS_PSM_CT16;
        }
        else if(Bitmap.InfoHeader.PSM == 24)
        {
                Texture->PSM = GS_PSM_CT24;
        }

	FTexSize = fioLseek(File, 0, SEEK_END);
	FTexSize -= Bitmap.FileHeader.Offset;

	fioLseek(File, Bitmap.FileHeader.Offset, SEEK_SET);
        
	u32 TextureSize = gsKit_texture_size(Texture->Width, Texture->Height, Texture->PSM);

	Texture->Mem = malloc(TextureSize);

        if(Bitmap.InfoHeader.PSM == 24)
        {
		image = malloc(FTexSize);
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
        } else {
		fioRead(File, Texture->Mem, TextureSize);
	}

        fioClose(File);

	Texture->Vram = gsKit_vram_alloc(gsGlobal, TextureSize);
	if(Texture->Vram == -1)
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

	jpg = jpgOpen(Path);
	if (jpg == NULL) {
		return -1;
		printf("error opening %s\n", Path);
	}
	
	TextureSize = (jpg->width * (jpg->color_components/8) * jpg->height);

	Texture->Mem = malloc(TextureSize);
	jpgReadImage(jpg, Texture->Mem);
	Texture->Width =  jpg->width;
	Texture->Height = jpg->height;
	Texture->PSM = GS_PSM_CT24;
	jpgClose(jpg);
	
	Texture->Vram = gsKit_vram_alloc(gsGlobal, TextureSize);
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
	int FileSize = gsKit_texture_size(Texture->Width, Texture->Height, Texture->PSM);
	Texture->Mem = malloc(FileSize);
	Texture->Vram = gsKit_vram_alloc(gsGlobal, FileSize);
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
	int size;
	int i;

	gsFont->Texture->Width  = data[1];
	gsFont->Texture->Height = data[2];
	gsFont->Texture->PSM    = data[3];
	gsFont->HChars          = data[4];
	gsFont->VChars          = data[5];
	gsFont->CharWidth       = data[6];
	gsFont->CharHeight      = data[7];

	size = gsKit_texture_size(gsFont->Texture->Width, gsFont->Texture->Height, gsFont->Texture->PSM);
	gsFont->Texture->Mem = malloc(size);
	gsFont->Texture->Vram = gsKit_vram_alloc(gsGlobal, size);
	memcpy(gsFont->Texture->Mem, &data[288/4], size);

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


int gsKit_texture_fnt(GSGLOBAL *gsGlobal, GSFONT *gsFont)
{
	u32 *mem;
	int File;
	int size;
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

	size = gsKit_texture_size(gsFont->Texture->Width, gsFont->Texture->Height, gsFont->Texture->PSM);
	gsFont->Texture->Mem = malloc(size);
	gsFont->Texture->Vram = gsKit_vram_alloc(gsGlobal, size);
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

void gsKit_texture_send(u8 *mem, int fbw, int width, int height, u32 tbp, u32 psm)
{
	u64* p_store;
	u64* p_data;
	u64* p_mem;
	int packets;
	int remain;
	int qwc;

	qwc = gsKit_texture_size(width, height, psm) / 16;

	packets = qwc / DMA_MAX_SIZE;
	remain  = qwc % DMA_MAX_SIZE;
	p_mem   = (u64*)mem;

	p_store = p_data = dmaKit_spr_alloc( (10+packets+(remain>0))*16 );

	FlushCache(0);

	// DMA DATA
	*p_data++ = DMA_TAG( 6, 0, DMA_CNT, 0, 0, 0 );
	*p_data++ = 0;

	*p_data++ = GIF_TAG( 4, 1, 0, 0, 0, 1 );
	*p_data++ = GIF_AD;

	*p_data++ = GS_SETREG_BITBLTBUF(0, 0, 0, tbp/256, fbw/64, psm);
	*p_data++ = GS_BITBLTBUF;

	*p_data++ = GS_SETREG_TRXPOS(0, 0, 0, 0, 0);
	*p_data++ = GS_TRXPOS;

	*p_data++ = GS_SETREG_TRXREG(width, height);
	*p_data++ = GS_TRXREG;

	*p_data++ = GS_SETREG_TRXDIR(0);
	*p_data++ = GS_TRXDIR;

	*p_data++ = GIF_TAG( qwc, 1, 0, 0, 2, 1 );
	*p_data++ = 0;

	while (packets-- > 0) {
	        *p_data++ = DMA_TAG( DMA_MAX_SIZE, 1, DMA_REF, 0, (u32)p_mem, 0 );
	        *p_data++ = 0;
			p_mem+= DMA_MAX_SIZE;
	}
	if (remain > 0) {
	        *p_data++ = DMA_TAG( remain, 1, DMA_REF, 0, (u32)p_mem, 0 );
	        *p_data++ = 0;
	}

	*p_data++ = DMA_TAG( 2, 0, DMA_END, 0, 0, 0 );
	*p_data++ = 0;

	*p_data++ = GIF_TAG( 1, 1, 0, 0, 0, 1 );
	*p_data++ = GIF_AD;

	*p_data++ = GS_TEXFLUSH;
	*p_data++ = 0;
	
	dmaKit_send_chain_spr( DMA_CHANNEL_GIF, 0, p_store);
}

void gsKit_texture_upload(GSGLOBAL *gsGlobal, GSTEXTURE *Texture)
{
	if (Texture->PSM == GS_PSM_T8) {
		gsKit_texture_send(Texture->Clut, gsGlobal->Width, 16, 16, Texture->VramClut, 0);
	}
	if (Texture->PSM == GS_PSM_T4) {
		gsKit_texture_send(Texture->Clut, gsGlobal->Width,  8,  2, Texture->VramClut, 0);
	}
	gsKit_texture_send(Texture->Mem, gsGlobal->Width, Texture->Width, Texture->Height, Texture->Vram, Texture->PSM);
}

static int log( int Value )
{
  int r = 0;
  
  Value--;

  while( Value > 0 )
  {
    Value = Value >> 1;
    r++;
  }
  
  return r;
}


void gsKit_prim_sprite_texture_3d(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, 	
				float x1, float y1, float z1, float u1, float v1,
				float x2, float y2, float z2, float u2, float v2, u64 color)
{
        u64* p_store;
        u64* p_data;
        int size = 8;

	int ix1 = gsKit_scale(gsGlobal, GS_AXIS_X, x1);
	int ix2 = gsKit_scale(gsGlobal, GS_AXIS_X, x2);
	int iy1 = gsKit_scale(gsGlobal, GS_AXIS_Y, y1);
	int iy2 = gsKit_scale(gsGlobal, GS_AXIS_Y, y2);
	int iz1 = gsKit_scale(gsGlobal, GS_AXIS_Z, z1);
	int iz2 = gsKit_scale(gsGlobal, GS_AXIS_Z, z2);
	
	int iu1 = gsKit_scale(gsGlobal, GS_MAP_U, u1);
	int iu2 = gsKit_scale(gsGlobal, GS_MAP_U, u2);
	int iv1 = gsKit_scale(gsGlobal, GS_MAP_V, v1);
	int iv2 = gsKit_scale(gsGlobal, GS_MAP_V, v2);
 
        if( gsGlobal->PrimAlphaEnable == 1 )
                size++;

        p_store = p_data = dmaKit_spr_alloc( size*16 );

        *p_data++ = GIF_TAG( size - 1, 1, 0, 0, 0, 1 );
        *p_data++ = GIF_AD;
        
        *p_data++ = GS_SETREG_TEX0(Texture->Vram/256, gsGlobal->Width/64, Texture->PSM,
                                   log(Texture->Width), log(Texture->Height), gsGlobal->PrimAlphaEnable, 0,
                                   Texture->VramClut/256, 0, 0, 0, 1);
        *p_data++ = GS_TEX0_1+gsGlobal->PrimContext;

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

        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}

void gsKit_prim_triangle_texture_3d(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, 	
				float x1, float y1, float z1, float u1, float v1,
				float x2, float y2, float z2, float u2, float v2,
				float x3, float y3, float z3, float u3, float v3, u64 color)
{
        u64* p_store;
        u64* p_data;
        int size = 10;

	int ix1 = gsKit_scale(gsGlobal, GS_AXIS_X, x1);
	int ix2 = gsKit_scale(gsGlobal, GS_AXIS_X, x2);
	int ix3 = gsKit_scale(gsGlobal, GS_AXIS_X, x3);
	int iy1 = gsKit_scale(gsGlobal, GS_AXIS_Y, y1);
	int iy2 = gsKit_scale(gsGlobal, GS_AXIS_Y, y2);
	int iy3 = gsKit_scale(gsGlobal, GS_AXIS_Y, y3);
	int iz1 = gsKit_scale(gsGlobal, GS_AXIS_Z, z1);
	int iz2 = gsKit_scale(gsGlobal, GS_AXIS_Z, z2);
	int iz3 = gsKit_scale(gsGlobal, GS_AXIS_Z, z3);
	
	int iu1 = gsKit_scale(gsGlobal, GS_MAP_U, u1);
	int iu2 = gsKit_scale(gsGlobal, GS_MAP_U, u2);
	int iu3 = gsKit_scale(gsGlobal, GS_MAP_U, u3);
	int iv1 = gsKit_scale(gsGlobal, GS_MAP_V, v1);
	int iv2 = gsKit_scale(gsGlobal, GS_MAP_V, v2);
	int iv3 = gsKit_scale(gsGlobal, GS_MAP_V, v3);
 
        if( gsGlobal->PrimAlphaEnable == 1 )
                size++;

        p_store = p_data = dmaKit_spr_alloc( size*16 );

        *p_data++ = GIF_TAG( size - 1, 1, 0, 0, 0, 1 );
        *p_data++ = GIF_AD;
        
        *p_data++ = GS_SETREG_TEX0(Texture->Vram/256, gsGlobal->Width/64, Texture->PSM,
                                   log(Texture->Width), log(Texture->Height), gsGlobal->PrimAlphaEnable, 0,
                                   Texture->VramClut/256, 0, 0, 0, 1);
        *p_data++ = GS_TEX0_1+gsGlobal->PrimContext;

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

        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}

void gsKit_prim_triangle_strip_texture(GSGLOBAL *gsGlobal, GSTEXTURE *Texture,
					float *TriStrip, int segments, float z, u64 color)
{
        u64* p_store;
        u64* p_data;
        int size = 4 + (segments * 2);
        int count;
        int vertexdata[segments*4];
 
        for(count = 0; count < (segments * 4); count+=4)
        {
                vertexdata[count] = gsKit_scale(gsGlobal, GS_AXIS_X, *TriStrip++);
                vertexdata[count+1] = gsKit_scale(gsGlobal, GS_AXIS_Y, *TriStrip++);
		vertexdata[count+2] = gsKit_scale(gsGlobal, GS_MAP_U, *TriStrip++);
		vertexdata[count+3] = gsKit_scale(gsGlobal, GS_MAP_V, *TriStrip++);
        }
	int iz = gsKit_scale(gsGlobal, GS_AXIS_Z, z);
	
        if( gsGlobal->PrimAlphaEnable == 1 )
                size++;

        p_store = p_data = dmaKit_spr_alloc( size*16 );

        *p_data++ = GIF_TAG( size - 1, 1, 0, 0, 0, 1 );
        *p_data++ = GIF_AD;
        
        *p_data++ = GS_SETREG_TEX0(Texture->Vram/256, gsGlobal->Width/64, Texture->PSM,
                                   log(Texture->Width), log(Texture->Height), gsGlobal->PrimAlphaEnable, 0,
                                   Texture->VramClut/256, 0, 0, 0, 1);
        *p_data++ = GS_TEX0_1+gsGlobal->PrimContext;

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
	
        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}

void gsKit_prim_triangle_strip_texture_3d(GSGLOBAL *gsGlobal, GSTEXTURE *Texture,
					float *TriStrip, int segments, u64 color)
{
        u64* p_store;
        u64* p_data;
        int size = 4 + (segments * 2);
        int count;
        int vertexdata[segments*5];
 
        for(count = 0; count < (segments * 5); count+=5)
        {
                vertexdata[count] = gsKit_scale(gsGlobal, GS_AXIS_X, *TriStrip++);
                vertexdata[count+1] = gsKit_scale(gsGlobal, GS_AXIS_Y, *TriStrip++);
		vertexdata[count+2] = gsKit_scale(gsGlobal, GS_AXIS_Z, *TriStrip++);
		vertexdata[count+3] = gsKit_scale(gsGlobal, GS_MAP_U, *TriStrip++);
		vertexdata[count+4] = gsKit_scale(gsGlobal, GS_MAP_V, *TriStrip++);
        }
	
        if( gsGlobal->PrimAlphaEnable == 1 )
                size++;

        p_store = p_data = dmaKit_spr_alloc( size*16 );

        *p_data++ = GIF_TAG( size - 1, 1, 0, 0, 0, 1 );
        *p_data++ = GIF_AD;
        
        *p_data++ = GS_SETREG_TEX0(Texture->Vram/256, gsGlobal->Width/64, Texture->PSM,
                                   log(Texture->Width), log(Texture->Height), gsGlobal->PrimAlphaEnable, 0,
                                   Texture->VramClut/256, 0, 0, 0, 1);
        *p_data++ = GS_TEX0_1+gsGlobal->PrimContext;

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
	
        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}

void gsKit_prim_triangle_fan_texture(GSGLOBAL *gsGlobal, GSTEXTURE *Texture,
					float *TriFan, int verticies, float z, u64 color)
{
        u64* p_store;
        u64* p_data;
        int size = 4 + (verticies * 2);
        int count;
        int vertexdata[verticies*4];
 
        for(count = 0; count < (verticies * 4); count+=4)
        {
                vertexdata[count] = gsKit_scale(gsGlobal, GS_AXIS_X, *TriFan++);
                vertexdata[count+1] = gsKit_scale(gsGlobal, GS_AXIS_Y, *TriFan++);
		vertexdata[count+2] = gsKit_scale(gsGlobal, GS_MAP_U, *TriFan++);
		vertexdata[count+3] = gsKit_scale(gsGlobal, GS_MAP_V, *TriFan++);
        }
	int iz = gsKit_scale(gsGlobal, GS_AXIS_Z, z);
	
        if( gsGlobal->PrimAlphaEnable == 1 )
                size++;

        p_store = p_data = dmaKit_spr_alloc( size*16 );

        *p_data++ = GIF_TAG( size - 1, 1, 0, 0, 0, 1 );
        *p_data++ = GIF_AD;
        
        *p_data++ = GS_SETREG_TEX0(Texture->Vram/256, gsGlobal->Width/64, Texture->PSM,
                                   log(Texture->Width), log(Texture->Height), gsGlobal->PrimAlphaEnable, 0,
                                   Texture->VramClut/256, 0, 0, 0, 1);
        *p_data++ = GS_TEX0_1+gsGlobal->PrimContext;

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
	
        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}

void gsKit_prim_triangle_fan_texture_3d(GSGLOBAL *gsGlobal, GSTEXTURE *Texture,
					float *TriFan, int verticies, u64 color)
{
        u64* p_store;
        u64* p_data;
        int size = 4 + (verticies * 2);
        int count;
        int vertexdata[verticies*5];
 
        for(count = 0; count < (verticies * 5); count+=5)
        {
                vertexdata[count] = gsKit_scale(gsGlobal, GS_AXIS_X, *TriFan++);
                vertexdata[count+1] = gsKit_scale(gsGlobal, GS_AXIS_Y, *TriFan++);
		vertexdata[count+2] = gsKit_scale(gsGlobal, GS_AXIS_Z, *TriFan++);
		vertexdata[count+3] = gsKit_scale(gsGlobal, GS_MAP_U, *TriFan++);
		vertexdata[count+4] = gsKit_scale(gsGlobal, GS_MAP_V, *TriFan++);
        }
	
        if( gsGlobal->PrimAlphaEnable == 1 )
                size++;

        p_store = p_data = dmaKit_spr_alloc( size*16 );

        *p_data++ = GIF_TAG( size - 1, 1, 0, 0, 0, 1 );
        *p_data++ = GIF_AD;
        
        *p_data++ = GS_SETREG_TEX0(Texture->Vram/256, gsGlobal->Width/64, Texture->PSM,
                                   log(Texture->Width), log(Texture->Height), gsGlobal->PrimAlphaEnable, 0,
                                   Texture->VramClut/256, 0, 0, 0, 1);
        *p_data++ = GS_TEX0_1+gsGlobal->PrimContext;

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
	
        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}

void gsKit_prim_quad_texture_3d(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, 	
				float x1, float y1, float z1, float u1, float v1,
				float x2, float y2, float z2, float u2, float v2,
				float x3, float y3, float z3, float u3, float v3,
				float x4, float y4, float z4, float u4, float v4, u64 color)
{
        u64* p_store;
        u64* p_data;
        int size = 12;

        int ix1 = gsKit_scale(gsGlobal, GS_AXIS_X, x1);
        int ix2 = gsKit_scale(gsGlobal, GS_AXIS_X, x2);
	int ix3 = gsKit_scale(gsGlobal, GS_AXIS_X, x3);
	int ix4 = gsKit_scale(gsGlobal, GS_AXIS_X, x4);
	
        int iy1 = gsKit_scale(gsGlobal, GS_AXIS_Y, y1);
        int iy2 = gsKit_scale(gsGlobal, GS_AXIS_Y, y2);
	int iy3 = gsKit_scale(gsGlobal, GS_AXIS_Y, y3);
	int iy4 = gsKit_scale(gsGlobal, GS_AXIS_Y, y4);

	int iz1 = gsKit_scale(gsGlobal, GS_AXIS_Z, z1);
	int iz2 = gsKit_scale(gsGlobal, GS_AXIS_Z, z2);
	int iz3 = gsKit_scale(gsGlobal, GS_AXIS_Z, z3);
	int iz4 = gsKit_scale(gsGlobal, GS_AXIS_Z, z4);
	
        int iu1 = gsKit_scale(gsGlobal, GS_MAP_U, u1);
        int iu2 = gsKit_scale(gsGlobal, GS_MAP_U, u2);
	int iu3 = gsKit_scale(gsGlobal, GS_MAP_U, u3);
	int iu4 = gsKit_scale(gsGlobal, GS_MAP_U, u4);
	
        int iv1 = gsKit_scale(gsGlobal, GS_MAP_V, v1);
        int iv2 = gsKit_scale(gsGlobal, GS_MAP_V, v2);
	int iv3 = gsKit_scale(gsGlobal, GS_MAP_V, v3);
	int iv4 = gsKit_scale(gsGlobal, GS_MAP_V, v4);
 
        if( gsGlobal->PrimAlphaEnable == 1 )
                size++;

        p_store = p_data = dmaKit_spr_alloc( size*16 );

        *p_data++ = GIF_TAG( size - 1, 1, 0, 0, 0, 1 );
        *p_data++ = GIF_AD;
        
        *p_data++ = GS_SETREG_TEX0(Texture->Vram/256, gsGlobal->Width/64, Texture->PSM,
                                   log(Texture->Width), log(Texture->Height), gsGlobal->PrimAlphaEnable, 0,
                                   Texture->VramClut/256, 0, 0, 0, 1);
        *p_data++ = GS_TEX0_1+gsGlobal->PrimContext;

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
	
        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}
