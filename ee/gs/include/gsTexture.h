//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsTexture.h - Header for gsTexture.c
//

#ifndef __GSTEXTURE_H__
#define __GSTEXTURE_H__

#include "gsKit.h"

struct gsTexture
{
        u32     Width;
        u32     Height;
        u32     PSM;
        void    *Mem;
        u32     *Clut;
        u32     Vram;
        u32     VramClut;
};
typedef struct gsTexture GSTEXTURE;

void gsKit_texture_png(GSTEXTURE *Texture, char *Path);
void gsKit_texture_jpeg(GSTEXTURE *Texture, char *Path);
void gsKit_texture_tga(GSTEXTURE *Texture, char *Path);
void gsKit_texture_rgb(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, char *Path);

void gsKit_texture_upload(GSTEXTURE *Texture);

#endif /* __GSTEXTURE_H__ */
