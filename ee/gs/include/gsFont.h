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

#ifdef __cplusplus
extern "C" {
#endif

s8 gsKit_font_upload(GSGLOBAL *gsGlobal, GSFONT *gsFont);
void gsKit_font_print(GSGLOBAL *gsGlobal, GSFONT *gsFont, int X, int Y, int Z, 
		      unsigned long color, char *String);
#ifdef __cplusplus
}
#endif

#endif /* __GSFONT_H__ */
