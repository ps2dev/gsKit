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

void gsKit_texture_upload(GSTEXTURE *Texture)
{
	u64* p_data       = NULL;
	u64* p_store      = NULL;

	p_data = p_store = dmaKit_spr_alloc( 6*16 );
	*p_data++ = DMA_CNT_TAG( 5 );
	*p_data++ = 0;

	*p_data++ = GIF_TAG( 4, 1, 0, 0, 0, 1 );
	*p_data++ = GIF_AD;

	*p_data++ = (u64)( ((Texture->PSM & 0x3f) << 24) | (((Texture->Width>>6) & 0x3f)<<16) |
	                   ((Texture->Vram/256) & 0x3fff) ) << 32;
	*p_data++ = GS_BITBLTBUF;

	*p_data++ = (unsigned long)( ((0)<<16) | (0) ) << 32;
	*p_data++ = GS_TRXPOS;

	*p_data++= ((unsigned long)(Texture->Width) << 32) | Texture->Width;
	*p_data++ = GS_TRXREG;

	*p_data++ = 0;
	*p_data++ = GS_TRXDIR;

        dmaKit_send_chain_spr( DMA_CHANNEL_GIF, 0, p_store);

	if( Texture->PSM == GS_PSM_CT32 )
	{
	}
	else if( Texture->PSM == GS_PSM_CT24 )
	{
	}
	else if( Texture->PSM == GS_PSM_CT16 )
	{
	}
	else if( Texture->PSM == GS_PSM_T8 )
	{
	}
	else if( Texture->PSM == GS_PSM_T4 )
	{
	}

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
