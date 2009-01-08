//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsFontM.h - Header for gsFontM.c
//

#ifndef __GSFONTM_H__
#define __GSFONTM_H__

#include "gsKit.h"

#define GSKIT_FONTM_MAXLINES 255

#define GSKIT_FALIGN_LEFT 0x00
#define GSKIT_FALIGN_CENTER 0x01
#define GSKIT_FALIGN_RIGHT 0x02

#define ALPHA_SRC 0
#define ALPHA_DST 1
#define ALPHA_ZERO 2
#define ALPHA_FIX 2
#define ALPHA(A,B,C,D,FIX) ( (((u64)(A))&3) | ((((u64)(B))&3)<<2) | ((((u64)(C))&3)<<4) | ((((u64)(D))&3)<<6) | ((((u64)(FIX)))<<32UL) )//(A - B)*C >> 7 + D

#define ALPHA_BLEND_NORMAL (ALPHA(ALPHA_SRC,ALPHA_DST,ALPHA_SRC,ALPHA_DST,0x00))
#define ALPHA_BLEND_ADD_NOALPHA    (ALPHA(ALPHA_SRC,ALPHA_ZERO,ALPHA_FIX,ALPHA_DST,0x80))
#define ALPHA_BLEND_ADD    (ALPHA(ALPHA_SRC,ALPHA_ZERO,ALPHA_SRC,ALPHA_DST,0x00))

#define GS_FONTM_PAGE_COUNT 2

struct gsKit_fontm_unpack
{
	unsigned int	size;
	unsigned int	data;
	unsigned int	ande;
	unsigned int	dif;
	unsigned int	shft;
	unsigned char	*ptr;
};

/// gsKit FONTM Header Structure
/// This stores the vital data for unpacked FONTM glyphsets.
struct gsKit_fontm_header
{
	u32	sig;
	u32	version;
	u32	bitsize;
	u32	baseoffset;
	u32	num_entries;
	u32	eof;
	u32	*offset_table;
};
typedef struct gsKit_fontm_header GSFONTMHDR;

/// gsKit Font Structure
/// This structure holds all relevant data for any
/// given font object, regardless of original format or type.
struct gsFontM
{
	GSTEXTURE *Texture;     ///< Font Texture Object
	GSFONTMHDR Header;      ///< FONTM Header
	u32 Vram[GS_FONTM_PAGE_COUNT];	///< FONTM VRAM Allocation (Double Buffered)
	u32 VramIdx;            ///< FONTM Current Double Buffer Index
	u32 LastPage[GS_FONTM_PAGE_COUNT];	///< FONTM Last Uploaded Texture Page
	u8 Align;               ///< FONTM Line Alignment
	float Spacing;          ///< FONTM Glyph Spacing
	void *TexBase;          ///< Glyphs Texture Base
    int pgcount;            /// Number of pages used in one call to gsKit_font_print_scaled
};
typedef struct gsFontM GSFONTM;


#ifdef __cplusplus
extern "C" {
#endif

/// Initialize Font Object
GSFONTM *gsKit_init_fontm();

int gsKit_fontm_upload(GSGLOBAL *gsGlobal, GSFONTM *gsFontM);
int gsKit_fontm_unpack(GSFONTM *gsFontM);
void gsKit_fontm_unpack_raw(unsigned char *base, struct gsKit_fontm_unpack *oke);
void gsKit_fontm_unpack_raw_1(struct gsKit_fontm_unpack *oke);
void gsKit_fontm_print_scaled(GSGLOBAL *gsGlobal, GSFONTM *gsFontM, float X, float Y, int Z,
		      float scale, unsigned long color, const char *String);

#define gsKit_fontm_print(gsGlobal, gsFontM, X, Y, Z, color, String) \
	gsKit_fontm_print_scaled(gsGlobal, gsFontM, X, Y, Z, 1.0f, color, String);

#ifdef __cplusplus
}
#endif

#endif /* __GSFONT_H__ */
