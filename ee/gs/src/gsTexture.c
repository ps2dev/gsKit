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

u32  gsKit_texture_size(GSTEXTURE *Texture)
{
	switch (Texture->PSM) {
		case GS_PSM_CT32:  return (Texture->Width*Texture->Height*4); break;
		case GS_PSM_CT24:  return (Texture->Width*Texture->Height*3); break;
		case GS_PSM_CT16:  return (Texture->Width*Texture->Height*2); break;
		case GS_PSM_CT16S: return (Texture->Width*Texture->Height*2); break;
		case GS_PSM_T8:    return (Texture->Width*Texture->Height  ); break;
		case GS_PSM_T4:    return (Texture->Width*Texture->Height/2); break;
		default: printf("gsKit: unsupported PSM %d\n", Texture->PSM);
	}

	return 0;
}

void gsKit_texture_png(GSTEXTURE *Texture, char *Path)
{
	printf("ERROR: gsKit_texture_png unimplimented.\n");
}

void gsKit_texture_jpeg(GSTEXTURE *Texture, char *Path)
{
	printf("ERROR: gsKit_texture_jpeg unimplimented.\n");
}

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

void gsKit_texture_upload(GSGLOBAL *gsGlobal, GSTEXTURE *Texture)
{
        u64* p_store;
        u64* p_data;
	u64* p_mem;
	int packets;
	int remain;
	int qwc;

	qwc  = gsKit_texture_size(Texture) / 16;

	packets = qwc / DMA_MAX_SIZE;
	remain  = qwc % DMA_MAX_SIZE;
	p_mem   = (u64*)Texture->Mem;

        p_store = p_data = dmaKit_spr_alloc( (10+packets+(remain>0))*16 );

	// DMA DATA
        *p_data++ = DMA_TAG( 6, 0, DMA_CNT, 0, 0, 0 );
        *p_data++ = 0;

        *p_data++ = GIF_TAG( 4, 1, 0, 0, 0, 1 );
        *p_data++ = GIF_AD;

	*p_data++ = GS_SETREG_BITBLTBUF(0, 0, 0, Texture->Vram, gsGlobal->Width/64, Texture->PSM);
	*p_data++ = GS_BITBLTBUF;

	*p_data++ = GS_SETREG_TRXPOS(0, 0, 0, 0, 0);
	*p_data++ = GS_TRXPOS;

	*p_data++ = GS_SETREG_TRXREG(Texture->Width, Texture->Height);
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

void gsKit_prim_sprite_texture(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, int x1, int y1, int u1, int v1,
                                                                       int x2, int y2, int u2, int v2,
                                                                       int z, u64 color)
{
        u64* p_store;
        u64* p_data;
        int size = 7;
        
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
        
        *p_data++ = GS_SETREG_TEX0(Texture->Vram/256, ((Texture->Width/64)&63), Texture->PSM,
                                   Texture->Width, Texture->Height, 0, 0,
                                   Texture->VramClut/256, 0, 1, 0, 1);
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
        
        *p_data++ = GS_SETREG_UV( u1, v1 );
        *p_data++ = GS_UV;

        *p_data++ = GS_SETREG_XYZ2( x1, y1, z );
        *p_data++ = GS_XYZ2;

        *p_data++ = GS_SETREG_UV( u2, v2 );
        *p_data++ = GS_UV;

        *p_data++ = GS_SETREG_XYZ2( x2, y2, z );
        *p_data++ = GS_XYZ2;

        dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}

