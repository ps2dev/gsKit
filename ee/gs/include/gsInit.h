//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsInit.h - Header for gsInit.c
//

#ifndef __GSINIT_H__
#define __GSINIT_H__

#include "gsKit.h"

#define GS_NONINTERLACED 0x00
#define GS_INTERLACED 0x01

#define GS_FIELD 0x00
#define GS_FRAME 0x01

#define GS_MODE_NTSC 0x02
#define GS_MODE_PAL  0x03

#define GS_MODE_VGA_640_60 0x1A
#define GS_MODE_VGA_640_72 0x1B
#define GS_MODE_VGA_640_75 0x1C
#define GS_MODE_VGA_640_85 0x1D

#define GS_MODE_VGA_800_56 0x2A
#define GS_MODE_VGA_800_60 0x2B
#define GS_MODE_VGA_800_72 0x2C
#define GS_MODE_VGA_800_75 0x2D
#define GS_MODE_VGA_800_85 0x2E

#define GS_MODE_VGA_1024_60 0x3B
#define GS_MODE_VGA_1024_70 0x3C
#define GS_MODE_VGA_1024_75 0x3D
#define GS_MODE_VGA_1024_85 0x3E

#define GS_MODE_DTV_480P  0x50
#define GS_MODE_DTV_1080I 0x51
#define GS_MODE_DTV_720P  0x52

#define GS_MODE_DVD_NTSC 0x72
#define GS_MODE_DVD_PAL  0x73
#define GS_MODE_DVD_480P 0x74

int gsKit_init(unsigned int interlace, unsigned int mode, unsigned int field);

#endif /* __GSINIT_H__ */
