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

struct gsKit_fontm_unpack
{
	unsigned int	size;
	unsigned int	data;
	unsigned int	ande;
	unsigned int	dif;
	unsigned int	shft;
	unsigned char	*ptr;
};

#ifdef __cplusplus
extern "C" {
#endif

int gsKit_font_upload(GSGLOBAL *gsGlobal, GSFONT *gsFont);
int gsKit_fontm_unpack(GSFONT *gsFont);
void gsKit_fontm_unpack_raw(unsigned char *base, struct gsKit_fontm_unpack *oke);
void gsKit_fontm_unpack_raw_1(struct gsKit_fontm_unpack *oke);
void gsKit_font_print_scaled(GSGLOBAL *gsGlobal, GSFONT *gsFont, float X, float Y, int Z,
		      float scale, unsigned long color, char *String);

#define gsKit_font_print(gsGlobal, gsFont, X, Y, Z, color, String) \
	gsKit_font_print_scaled(gsGlobal, gsFont, X, Y, Z, 1.0f, color, String);

#ifdef __cplusplus
}
#endif

#endif /* __GSFONT_H__ */
