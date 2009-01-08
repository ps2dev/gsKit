//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsToolkit.h - Header for gsToolkit.c
//

#ifndef __GSTOOLKIT_H__
#define __GSTOOLKIT_H__

#include "gsKit.h"

struct gsBitMapFileHeader
{
	u16	Type;
	u32	Size;
	u16 Reserved1;
	u16 Reserved2;
	u32 Offset;
} __attribute__ ((packed));
typedef struct gsBitMapFileHeader GSBMFHDR;

struct gsBitMapInfoHeader
{
	u32	Size;
	u32	Width;
	u32	Height;
	u16	Planes;
	u16 BitCount;
	u32 Compression;
	u32 SizeImage;
	u32 XPelsPerMeter;
	u32 YPelsPerMeter;
	u32 ColorUsed;
	u32 ColorImportant;
} __attribute__ ((packed));
typedef struct gsBitMapInfoHeader GSBMIHDR;

struct gsBitMapClut
{
	u8 Blue;
	u8 Green;
	u8 Red;
	u8 Alpha;
} __attribute__ ((packed));
typedef struct gsBitMapClut GSBMCLUT;

struct gsBitmap
{
	GSBMFHDR FileHeader;
	GSBMIHDR InfoHeader;
	char *Texture;
	GSBMCLUT *Clut;
};
typedef struct gsBitmap GSBITMAP;

//remove fontm specific things here
#define GSKIT_FTYPE_FNT 0x00
#define GSKIT_FTYPE_BMP_DAT 0x01
#define GSKIT_FTYPE_PNG_DAT 0x02

/// gsKit Font Structure
/// This structure holds all relevant data for any
/// given font object, regardless of original format or type.
struct gsFont
{
	char *Path;		///< Path (string) to the Font File
	char *Path_DAT;		///< Path (string) to the Glyph DAT File
	u8 Type;		///< Font Type
	u8 *RawData;		///< Raw File Data
	int RawSize;		///< Raw File Datasize
	GSTEXTURE *Texture;	///< Font Texture Object
	u32 CharWidth;		///< Character Width
	u32 CharHeight;		///< Character Height
	u32 HChars;		///< Character Rows
	u32 VChars;		///< Character Columns
	s16 *Additional;		///< Additional Font Data
    int pgcount;    /// Number of pages used in one call to gsKit_font_print_scaled
};
typedef struct gsFont GSFONT;

#ifdef __cplusplus
extern "C" {
#endif

int gsKit_texture_bmp(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, char *Path);
int gsKit_texture_png(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, char *Path);
int gsKit_texture_jpeg(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, char *Path);
int gsKit_texture_tiff(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, char *Path);
int gsKit_texture_tga(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, char *Path);
int gsKit_texture_raw(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, char *Path);

/// Initialize Font Object (From Memory)
GSFONT *gsKit_init_font(u8 type, char *path);
GSFONT *gsKit_init_font_raw(u8 type, u8 *data, int size);

int gsKit_font_upload(GSGLOBAL *gsGlobal, GSFONT *gsFont);
int gsKit_font_upload_raw(GSGLOBAL *gsGlobal, GSFONT *gsFont);

//int gsKit_texture_fnt(GSGLOBAL *gsGlobal, GSFONT *gsFont);
int gsKit_texture_fnt_raw(GSGLOBAL *gsGlobal, GSFONT *gsFont);

void gsKit_font_print_scaled(GSGLOBAL *gsGlobal, GSFONT *gsFont, float X, float Y, int Z,
		      float scale, unsigned long color, const char *String);

#define gsKit_font_print(gsGlobal, gsFont, X, Y, Z, color, String) \
	gsKit_font_print_scaled(gsGlobal, gsFont, X, Y, Z, 1.0f, color, String);

#ifdef __cplusplus
}
#endif

#endif /* __GSTOOLKIT_H__ */
