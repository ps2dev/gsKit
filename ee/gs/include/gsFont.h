//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsFont.h - Header for gsFont.c
//

#ifndef __GSFONT_H__
#define __GSFONT_H__

#include "gsKit.h"

#define GSKIT_FTYPE_FNT 0x00
#define GSKIT_FTYPE_TTF 0x01
#define GSKIT_FTYPE_BMP_DAT 0x02
#define GSKIT_FTYPE_FONTM 0x03

#define ALPHA_SRC 0
#define ALPHA_DST 1
#define ALPHA_ZERO 2
#define ALPHA_FIX 2
#define ALPHA(A,B,C,D,FIX) ( (((u64)(A))&3) | ((((u64)(B))&3)<<2) | ((((u64)(C))&3)<<4) | ((((u64)(D))&3)<<6) | ((((u64)(FIX)))<<32UL) )//(A - B)*C >> 7 + D 

#define ALPHA_BLEND_NORMAL (ALPHA(ALPHA_SRC,ALPHA_DST,ALPHA_SRC,ALPHA_DST,0x00))
#define ALPHA_BLEND_ADD_NOALPHA    (ALPHA(ALPHA_SRC,ALPHA_ZERO,ALPHA_FIX,ALPHA_DST,0x80))
#define ALPHA_BLEND_ADD    (ALPHA(ALPHA_SRC,ALPHA_ZERO,ALPHA_SRC,ALPHA_DST,0x00))

#ifdef __cplusplus
extern "C" {
#endif

int gsKit_font_upload(GSGLOBAL *gsGlobal, GSFONT *gsFont);
void gsKit_font_print(GSGLOBAL *gsGlobal, GSFONT *gsFont, int X, int Y, int Z, 
		      unsigned long color, char *String);

#ifdef __cplusplus
}
#endif

#endif /* __GSFONT_H__ */
