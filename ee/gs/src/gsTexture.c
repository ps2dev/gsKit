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

}


