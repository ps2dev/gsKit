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

#include "gsKit.h"

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

	return 0;
}

void gsKit_texture_png(GSTEXTURE *Texture, char *Path)
{
	printf("ERROR: gsKit_texture_png unimplimented.\n");
}

void gsKit_texture_jpeg(GSTEXTURE *Texture, char *Path)
{
}
#if 0
int  gsKit_texture_jpeg(GSTEXTURE *Texture, char *Path)
{
	jpgData *jpg;

	jpg = jpgOpen(Path);
	if (jpg == NULL) {
		return -1;
		printf("error opening %s\n", Path);
	}

	Texture->Mem = malloc(jpg->width * jpg->color_components * jpg->height);
	jpgReadImage(jpg, Texture->Mem);
	Texture->Width =  jpg->width;
	Texture->Height = jpg->height;
	Texture->PSM = GS_PSM_CT24;
	jpgClose(jpg);
	free(data);
}
#endif

void gsKit_texture_tga(GSTEXTURE *Texture, char *Path)
{
	printf("ERROR: gsKit_texture_tga unimplimented.\n");
}

void gsKit_texture_rgb(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, char *Path)
{
	switch( Texture->PSM )
	{
		case GS_PSM_CT32 : Texture->Vram = gsKit_vram_alloc(gsGlobal, Texture->Width*Texture->Height * 4); break;
		case GS_PSM_T8 : Texture->Vram = gsKit_vram_alloc(gsGlobal, Texture->Width*Texture->Height); break;
		case GS_PSM_T4 : Texture->Vram = gsKit_vram_alloc(gsGlobal, Texture->Width*Texture->Height / 4); break;
	}
}

void gsKit_texture_send(u8 *mem, int fbw, int width, int height, u32 tbp, u32 psm)
{
        u64* p_store;
        u64* p_data;
	u64* p_mem;
	int packets;
	int remain;
	int qwc;

	qwc  = gsKit_texture_size(width, height, psm) / 16;

	packets = qwc / DMA_MAX_SIZE;
	remain  = qwc % DMA_MAX_SIZE;
	p_mem   = (u64*)mem;

        p_store = p_data = dmaKit_spr_alloc( (10+packets+(remain>0))*16 );

	// DMA DATA
        *p_data++ = DMA_TAG( 6, 0, DMA_CNT, 0, 0, 0 );
        *p_data++ = 0;

        *p_data++ = GIF_TAG( 4, 1, 0, 0, 0, 1 );
        *p_data++ = GIF_AD;

	*p_data++ = GS_SETREG_BITBLTBUF(0, 0, 0, tbp, fbw/64, psm);
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

	*p_data++ = 0;
	*p_data++ = GS_TEXFLUSH;

        dmaKit_send_chain_spr( DMA_CHANNEL_GIF, 0, p_store);
}

void gsKit_texture_upload(GSGLOBAL *gsGlobal, GSTEXTURE *Texture)
{
	if (Texture->PSM == GS_PSM_T8) {
		gsKit_texture_send(Texture->Clut, gsGlobal->Width, 16, 16, Texture->VramClut/256, 0);
	}
	if (Texture->PSM == GS_PSM_T4) {
		gsKit_texture_send(Texture->Clut, gsGlobal->Width,  8,  2, Texture->VramClut/256, 0);
	}
	gsKit_texture_send(Texture->Mem, gsGlobal->Width, Texture->Width, Texture->Height, Texture->Vram/256, Texture->PSM);
}

int log( int Value )
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


void gsKit_prim_sprite_texture(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, int x1, int y1, int u1, int v1,
                                                                       int x2, int y2, int u2, int v2,
                                                                       int z, u64 color)
{
        u64* p_store;
        u64* p_data;
        int size = 8;
        
        x1 = gsKit_scale(gsGlobal, GS_AXIS_X, x1);
        x2 = gsKit_scale(gsGlobal, GS_AXIS_X, x2);
        y1 = gsKit_scale(gsGlobal, GS_AXIS_Y, y1);
        y2 = gsKit_scale(gsGlobal, GS_AXIS_Y, y2);
 
        x1 += gsGlobal->OffsetX << 4;
        x2 += gsGlobal->OffsetX << 4;

        y1 += gsGlobal->OffsetY << 4;
        y2 += gsGlobal->OffsetY << 4;
        
        if( gsGlobal->PrimAlphaEnable == 1 )
                size++;

        p_store = p_data = dmaKit_spr_alloc( size*16 );

        *p_data++ = GIF_TAG( size - 1, 1, 0, 0, 0, 1 );
        *p_data++ = GIF_AD;
        
        *p_data++ = GS_SETREG_TEX0(Texture->Vram/256, ((gsGlobal->Width/64)&63), Texture->PSM,
                                   log(Texture->Width), log(Texture->Height), 0, 0,
                                   Texture->VramClut/256, 0, 0, 0, 1);
        *p_data++ = GS_TEX0_1+gsGlobal->PrimContext;

        if( gsGlobal->PrimAlphaEnable == 1 )
        {
                *p_data++ = gsGlobal->PrimAlpha;
                *p_data++ = GS_ALPHA_1+gsGlobal->PrimContext;
        }
 
        *p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_SPRITE, 0, 1, gsGlobal->PrimFogEnable,
                                    0/*gsGlobal->PrimAlphaEnable*/, gsGlobal->PrimAAEnable,
                                    1, gsGlobal->PrimContext, 0);
        
        *p_data++ = GS_PRIM;
        
        *p_data++ = color;
        *p_data++ = GS_RGBAQ;
        
        *p_data++ = GS_SETREG_UV( u1<<4, v1<<4 );
        *p_data++ = GS_UV;

        *p_data++ = GS_SETREG_XYZ2( x1, y1, z );
        *p_data++ = GS_XYZ2;

        *p_data++ = GS_SETREG_UV( u2<<4, v2<<4 );
        *p_data++ = GS_UV;

        *p_data++ = GS_SETREG_XYZ2( x2, y2, z );
        *p_data++ = GS_XYZ2;

        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}

