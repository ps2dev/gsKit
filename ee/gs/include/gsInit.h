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
// Parts taken from emoon's BreakPoint Demo Library
//

#ifndef __GSINIT_H__
#define __GSINIT_H__

#include "gsKit.h"

/// Persistent Draw Queue
#define GS_PERSISTENT 0x00
/// Oneshot Draw Queue
#define GS_ONESHOT 0x01

/// Size of Persistent drawbuffer (Single Buffered)
#define GS_RENDER_QUEUE_PER_POOLSIZE 1024 * 256 // 256K of persistent renderqueue
/// Size of Oneshot drawbuffer (Double Buffered, so it uses this size * 2)
#define GS_RENDER_QUEUE_OS_POOLSIZE 1024 * 1024 // 1024K of oneshot renderqueue

/// Draw Persistent objects first, before oneshot objects
#define GS_PER_OS 0x00
/// Draw Persistent objects last, after oneshot objects
#define GS_OS_PER 0x01

/// Non-Interlaced Mode
#define GS_NONINTERLACED 0x00
/// Interlaced Mode
#define GS_INTERLACED 0x01

/// Field Mode - Reads Every Other Line
#define GS_FIELD 0x00
/// Frame Mode - Reads Every Line
#define GS_FRAME 0x01

/// NTSC Full Buffer
#define GS_MODE_NTSC 0x02
/// PAL Full Buffer
#define GS_MODE_PAL  0x03

/// NTSC Half Buffer
#define GS_MODE_NTSC_I 0x04
/// PAL Half Buffer
#define GS_MODE_PAL_I 0x05
/// DTV 1080I Half Buffer
#define GS_MODE_DTV_1080I_I 0x06

/// Auto-detect Full Buffer
#define GS_MODE_AUTO  0x07
/// Auto-detect Half Buffer
#define GS_MODE_AUTO_I 0x08

/// 4:3 Aspect Ratio (UNUSED)
#define GS_ASPECT_4_3 0x0
/// 16:9 Aspect Ratio (UNUSED)
#define GS_ASPECT_16_9 0x1

/// VGA 640x480 @ 60Hz
#define GS_MODE_VGA_640_60 0x1A
/// VGA 640x480 @ 72Hz
#define GS_MODE_VGA_640_72 0x1B
/// VGA 640x480 @ 65Hz
#define GS_MODE_VGA_640_75 0x1C
/// VGA 640x480 @ 85Hz
#define GS_MODE_VGA_640_85 0x1D

/// VGA 800x600 @ 56Hz
#define GS_MODE_VGA_800_56 0x2A
/// VGA 800x600 @ 60Hz
#define GS_MODE_VGA_800_60 0x2B
/// VGA 800x600 @ 72Hz
#define GS_MODE_VGA_800_72 0x2C
/// VGA 800x600 @ 75Hz
#define GS_MODE_VGA_800_75 0x2D
/// VGA 800x600 @ 85Hz
#define GS_MODE_VGA_800_85 0x2E

/// VGA 1024x768 @ 60Hz
#define GS_MODE_VGA_1024_60 0x3B
/// VGA 1024x768 @ 70Hz
#define GS_MODE_VGA_1024_70 0x3C
/// VGA 1024x768 @ 75Hz
#define GS_MODE_VGA_1024_75 0x3D
/// VGA 1024x768 @ 85Hz
#define GS_MODE_VGA_1024_85 0x3E

/// VGA 1280x1024 @ 60Hz
#define GS_MODE_VGA_1280_60 0x4A
/// VGA 1280x1024 @ 75Hz
#define GS_MODE_VGA_1280_75 0x4B

/// DTV 480 Progressive Scan (720x480)
#define GS_MODE_DTV_480P  0x50
/// DTV 1080 Interlaced (1920x1080)
#define GS_MODE_DTV_1080I 0x51
/// DTV 720 Progressive Scan (1280x720)
#define GS_MODE_DTV_720P  0x52

/// DVD NTSC
#define GS_MODE_DVD_NTSC 0x72
/// DVD PAL
#define GS_MODE_DVD_PAL  0x73
/// DVD 480P
#define GS_MODE_DVD_480P 0x74

/// R8 G8 B8 A8 (RGBA32) Texture
#define GS_PSM_CT32 0x00
/// R8 G8 B8 (RGB24) Texture
#define GS_PSM_CT24 0x01
/// RGBA16 Texture
#define GS_PSM_CT16 0x02
/// RGBA16 Texture ?
#define GS_PSM_CT16S 0x0A
/// 8 Bit Texture with CLUT
#define GS_PSM_T8 0x13
/// 8 Bit Texture with CLUT ?
#define GS_PSM_T8H 0x1B
/// 4 Bit Texture with CLUT
#define GS_PSM_T4 0x14
/// 4 Bit Texture with CLUT ?
#define GS_PSM_T4HL 0x24
/// 4 Bit Texture with CLUT ?
#define GS_PSM_T4HH 0x2C

/// R8 G8 B8 A8 (RGBA32) ZBuffer
#define GS_PSMZ_32 0x00
/// R8 G8 B8 (RGB24) ZBuffer
#define GS_PSMZ_24 0x01
/// RGBA16 ZBuffer
#define GS_PSMZ_16 0x02
/// RGBA16 ZBuffer ?
#define GS_PSMZ_16S 0x0A

/// Blend Alpha Primitives "Front To Back"
#define GS_BLEND_FRONT2BACK 0x12
/// Blend Alpha Primitives "Back To Front"
#define GS_BLEND_BACK2FRONT 0x01

/// Define a setting to be OFF
#define GS_SETTING_OFF 0x00

/// Define a setting to be ON
#define GS_SETTING_ON 0x01

// Begin Register List

/// GS Primitive
#define GS_PRIM       0x00
/// GS Color in RGBAQ Format
#define GS_RGBAQ      0x01
/// GS Texture Coordinate Value in Texture Coordinates
#define GS_ST         0x02
/// GS Texture Coordinate Value in Texel Coordinates
#define GS_UV         0x03
/// GS Vertex Coordinate Value With Fog Coefficent
#define GS_XYZF2      0x04
/// GS Vertex Coordinate Value
#define GS_XYZ2       0x05
/// GS Texture Information Settings (Context 1)
#define GS_TEX0_1     0x06
/// GS Texture Information Settings (Context 2)
#define GS_TEX0_2     0x07
/// GS Texture Wrap Mode (Context 1)
#define GS_CLAMP_1    0x08
/// GS Texture Wrap Mode (Context 2)
#define GS_CLAMP_2    0x09
/// GS Vertex Fog Value
#define GS_FOG        0x0a
/// GS Vertex Coordinate Value With Fog Coefficent (Without Drawing Kick)
#define GS_XYZF3      0x0c
/// GS Vertex Coordinate Value (Without Drawing Kick)
#define GS_XYZ3       0x0d
/// GS Texture Information Settings (Context 1)
#define GS_TEX1_1     0x14
/// GS Texture Information Settings (Context 2)
#define GS_TEX1_2     0x15
/// GS Texture Information Settings (Context 1)
#define GS_TEX2_1     0x16
/// GS Texture Information Settings (Context 2)
#define GS_TEX2_2     0x17
/// GS Offset Value
#define GS_XYOFFSET_1 0x18
/// GS Offset Value
#define GS_XYOFFSET_2 0x19
/// GS Primitive Attribute Setting Mode
#define GS_PRMODECONT 0x1a
/// GS Primitive Mode
#define GS_PRMODE     0x1b
/// GS Texture CLUT Location
#define GS_TEXCLUT    0x1c
/// GS Raster Address Mask
#define GS_SCANMSK    0x22
/// GS MIPMAP Information for Levels 1-3 (Context 1)
#define GS_MIPTBP1_1  0x34
/// GS MIPMAP Information for Levels 1-3 (Context 2)
#define GS_MIPTBP1_2  0x35
/// GS MIPMAP Information for Levels 4-6 (Context 1)
#define GS_MIPTBP2_1  0x36
/// GS MIPMAP Information for Levels 4-6 (Context 2)
#define GS_MIPTBP2_2  0x37
/// GS Texture Alpha Value
#define GS_TEXA       0x3b
/// GS Distant Fog Color
#define GS_FOGCOL     0x3d
/// GS Texture Page Buffer
#define GS_TEXFLUSH   0x3f
/// GS Scissoring Area (Context 1)
#define GS_SCISSOR_1  0x40
/// GS Scissoring Area (Context 2)
#define GS_SCISSOR_2  0x41
/// GS Alpha Blending (Context 1)
#define GS_ALPHA_1    0x42
/// GS Alpha Blending (Context 2)
#define GS_ALPHA_2    0x43
/// GS Dither Matrix
#define GS_DIMX       0x44
/// GS Dither Control
#define GS_DTHE       0x45
/// GS Color Clamp
#define GS_COLCLAMP   0x46
/// GS TEST (Context 1)
#define GS_TEST_1     0x47
/// GS TEST (Context 2)
#define GS_TEST_2     0x48
/// GS Alpha Blending (In Pixel Units)
#define GS_PABE       0x49
/// GS Alpha Correction Value (Context 1)
#define GS_FBA_1      0x4a
/// GS Alpha Correction Value (Context 2)
#define GS_FBA_2      0x4b
/// GS Frame Buffer (Context 1)
#define GS_FRAME_1    0x4c
/// GS Frame Buffer (Context 2)
#define GS_FRAME_2    0x4d
/// GS Z Buffer (Context 1)
#define GS_ZBUF_1     0x4e
/// GS Z Buffer (Context 2)
#define GS_ZBUF_2     0x4f
/// GS Setting for Transmission Between Buffers
#define GS_BITBLTBUF  0x50
/// GS Specificiation for Transmission Area in Buffers
#define GS_TRXPOS     0x51
/// GS Specificiation for Transmission Area in Buffers
#define GS_TRXREG     0x52
/// GS Activation for Transmission Between Buffers
#define GS_TRXDIR     0x53
/// Data Port for Transmission Between Buffers
#define GS_HWREG      0x54
/// SIGNAL Event Occourence
#define GS_SIGNAL     0x60
/// FINISH Event Occourence
#define GS_FINISH     0x61
/// LABEL Event Occourence
#define GS_LABEL      0x62

// End Register List

/// GS Point Primitive
#define GS_PRIM_PRIM_POINT	0x00
/// GS Line Primitive
#define GS_PRIM_PRIM_LINE	0x01
/// GS Line Strip Primitive
#define GS_PRIM_PRIM_LINESTRIP	0x02
/// GS Triangle Primitive
#define GS_PRIM_PRIM_TRIANGLE	0x03
/// GS Triangle Strip Primitive
#define GS_PRIM_PRIM_TRISTRIP	0x04
/// GS Triangle Fan Primitive
#define GS_PRIM_PRIM_TRIFAN	0x05
/// GS Sprite Primitive
#define GS_PRIM_PRIM_SPRITE	0x06

/// GIFTAG Reglist Prim Identifiers
#define GIF_PRIM_POINT			0x1000
#define GIF_PRIM_LINE			0x1100
#define GIF_PRIM_LINESTRIP		0x1200
#define GIF_PRIM_TRIANGLE		0x1300
#define GIF_PRIM_TRISTRIP		0x1400
#define GIF_PRIM_TRIFAN			0x1500
#define GIF_PRIM_SPRITE			0x1600
#define GIF_PRIM_QUAD			0x1700
#define GIF_PRIM_TRIANGLE_GOURAUD	0x1800
#define GIF_PRIM_QUAD_GOURAUD		0x1900
#define GIF_PRIM_SPRITE_TEXTURED	0x1A00
#define GIF_PRIM_TRIANGLE_TEXTURED	0x1B00
#define GIF_PRIM_TRISTRIP_TEXTURED	0x1C00
#define GIF_PRIM_TRIFAN_TEXTURED	0x1D00
#define GIF_PRIM_QUAD_TEXTURED		0x1E00

// DMA Packet Header "GIF Tag" For Packets Sent To GIF

// A+D Data
/// A+D GIFTAG
#define GIF_TAG_AD(NLOOP)   \
		((u64)(NLOOP)		<< 0)	| \
		((u64)(1)		<< 15)	| \
		((u64)(0)		<< 46)	| \
		((u64)(0)		<< 47)	| \
		((u64)(0)		<< 58)	| \
		((u64)(1)		<< 60);
// Point Primitive
/// Point Primitive GIFTAG
#define GIF_TAG_POINT(NLOOP)   \
		((u64)(NLOOP)		<< 0)	| \
		((u64)(1)		<< 15)	| \
		((u64)(0)		<< 46)	| \
		((u64)(0)		<< 47)	| \
		((u64)(1)		<< 58)	| \
		((u64)(4)		<< 60);
/// Point Primitive REGLIST
#define GIF_TAG_POINT_REGS   \
		((u64)(GS_PRIM)		<< 0)	| \
		((u64)(GS_RGBAQ)	<< 4)	| \
		((u64)(GS_XYZ2)		<< 8)	| \
		((u64)(GIF_NOP)		<< 12);

// Line Primitive
/// Line Primitive GIFTAG
#define GIF_TAG_LINE(NLOOP)   \
		((u64)(NLOOP)		<< 0)	| \
		((u64)(1)		<< 15)	| \
		((u64)(0)		<< 46)	| \
		((u64)(0)		<< 47)	| \
		((u64)(1)		<< 58)	| \
		((u64)(4)		<< 60);
/// Line Primitive REGLIST
#define GIF_TAG_LINE_REGS   \
		((u64)(GS_PRIM)		<< 0)	| \
		((u64)(GS_RGBAQ)	<< 4)	| \
		((u64)(GS_XYZ2)		<< 8)	| \
		((u64)(GS_XYZ2)		<< 12);

// Triangle Primitive
/// Triangle Primitive GIFTAG
#define GIF_TAG_TRIANGLE(NLOOP)   \
		((u64)(NLOOP)		<< 0)	| \
		((u64)(1)		<< 15)	| \
		((u64)(0)		<< 46)	| \
		((u64)(0)		<< 47)	| \
		((u64)(1)		<< 58)	| \
		((u64)(6)		<< 60);
/// Triangle Primitive REGLIST
#define GIF_TAG_TRIANGLE_REGS   \
		((u64)(GS_PRIM)		<< 0)	| \
		((u64)(GS_RGBAQ)	<< 4)	| \
		((u64)(GS_XYZ2)		<< 8)	| \
		((u64)(GS_XYZ2)		<< 12)	| \
		((u64)(GS_XYZ2)		<< 16)	| \
		((u64)(GIF_NOP)		<< 20);

// Sprite Primitive
/// Sprite Primitive GIFTAG
#define GIF_TAG_SPRITE(NLOOP)   \
		((u64)(NLOOP)		<< 0)	| \
		((u64)(1)		<< 15)	| \
		((u64)(0)		<< 46)	| \
		((u64)(0)		<< 47)	| \
		((u64)(1)		<< 58)	| \
		((u64)(4)		<< 60);
/// Sprite Primitive REGLIST
#define GIF_TAG_SPRITE_REGS   \
		((u64)(GS_PRIM)		<< 0)	| \
		((u64)(GS_RGBAQ)	<< 4)	| \
		((u64)(GS_XYZ2)		<< 8)	| \
		((u64)(GS_XYZ2)		<< 12);

// Quad Primitive
/// Quad Primitive GIFTAG
#define GIF_TAG_QUAD(NLOOP)   \
		((u64)(NLOOP)		<< 0)	| \
		((u64)(1)		<< 15)	| \
		((u64)(0)		<< 46)	| \
		((u64)(0)		<< 47)	| \
		((u64)(1)		<< 58)	| \
		((u64)(6)		<< 60);
/// Quad Primitive REGLIST
#define GIF_TAG_QUAD_REGS   \
		((u64)(GS_PRIM)		<< 0)	| \
		((u64)(GS_RGBAQ)	<< 4)	| \
		((u64)(GS_XYZ2)		<< 8)	| \
		((u64)(GS_XYZ2)		<< 12)	| \
		((u64)(GS_XYZ2)		<< 16)	| \
		((u64)(GS_XYZ2)		<< 20);

// Gouraud Shaded Triangle Primitive
/// Gouraud Shaded Triangle Primitive GIFTAG
#define GIF_TAG_TRIANGLE_GOURAUD(NLOOP)   \
		((u64)(NLOOP)		<< 0)	| \
		((u64)(1)		<< 15)	| \
		((u64)(0)		<< 46)	| \
		((u64)(0)		<< 47)	| \
		((u64)(1)		<< 58)	| \
		((u64)(8)		<< 60);
/// Gouraud Shaded Triangle Primitive REGLIST
#define GIF_TAG_TRIANGLE_GOURAUD_REGS   \
		((u64)(GS_PRIM)		<< 0)	| \
		((u64)(GS_RGBAQ)	<< 4)	| \
		((u64)(GS_XYZ2)		<< 8)	| \
		((u64)(GS_RGBAQ)	<< 12)	| \
		((u64)(GS_XYZ2)		<< 16)	| \
		((u64)(GS_RGBAQ)	<< 20)	| \
		((u64)(GS_XYZ2)		<< 24)	| \
		((u64)(GIF_NOP)		<< 28);

// Gouraud Shaded Quad Primitive
/// Gouraud Shaded Quad Primitive GIFTAG
#define GIF_TAG_QUAD_GOURAUD(NLOOP)   \
		((u64)(NLOOP)		<< 0)	| \
		((u64)(1)		<< 15)	| \
		((u64)(0)		<< 46)	| \
		((u64)(0)		<< 47)	| \
		((u64)(1)		<< 58)	| \
		((u64)(10)		<< 60);
/// Gouraud Shaded Quad Primitive REGLIST
#define GIF_TAG_QUAD_GOURAUD_REGS   \
		((u64)(GS_PRIM)		<< 0)	| \
		((u64)(GS_RGBAQ)	<< 4)	| \
		((u64)(GS_XYZ2)		<< 8)	| \
		((u64)(GS_RGBAQ)	<< 12)	| \
		((u64)(GS_XYZ2)		<< 16)	| \
		((u64)(GS_RGBAQ)	<< 20)	| \
		((u64)(GS_XYZ2)		<< 24)	| \
		((u64)(GS_RGBAQ)	<< 28)	| \
		((u64)(GS_XYZ2)		<< 32)	| \
		((u64)(GS_RGBAQ)	<< 36);

// Textured Sprite Primitive
/// Textured Sprite Primitive GIFTAG
#define GIF_TAG_SPRITE_TEXTURED(NLOOP)   \
		((u64)(NLOOP)		<< 0)	| \
		((u64)(1)		<< 15)	| \
		((u64)(0)		<< 46)	| \
		((u64)(0)		<< 47)	| \
		((u64)(1)		<< 58)	| \
		((u64)(8)		<< 60);
/// Textured Sprite Primitive REGLIST
#define GIF_TAG_SPRITE_TEXTURED_REGS(ctx)   \
		((u64)(GS_TEX0_1 + ctx)	<< 0)	| \
		((u64)(GS_PRIM)		<< 4)	| \
		((u64)(GS_RGBAQ)	<< 8)	| \
		((u64)(GS_UV)		<< 12)	| \
		((u64)(GS_XYZ2)		<< 16)	| \
		((u64)(GS_UV)		<< 20)	| \
		((u64)(GS_XYZ2)		<< 24)	| \
		((u64)(GIF_NOP)		<< 28);

// Textured Triangle Primitive
/// Textured Triangle Primitive GIFTAG
#define GIF_TAG_TRIANGLE_TEXTURED(NLOOP)   \
		((u64)(NLOOP)		<< 0)	| \
		((u64)(1)		<< 15)	| \
		((u64)(0)		<< 46)	| \
		((u64)(0)		<< 47)	| \
		((u64)(1)		<< 58)	| \
		((u64)(10)		<< 60);
/// Textured Triangle Primitive REGLIST
#define GIF_TAG_TRIANGLE_TEXTURED_REGS(ctx)   \
		((u64)(GS_TEX0_1 + ctx)	<< 0)	| \
		((u64)(GS_PRIM)		<< 4)	| \
		((u64)(GS_RGBAQ)	<< 8)	| \
		((u64)(GS_UV)		<< 12)	| \
		((u64)(GS_XYZ2)		<< 16)	| \
		((u64)(GS_UV)		<< 20)	| \
		((u64)(GS_XYZ2)		<< 24)	| \
		((u64)(GS_UV)		<< 28)	| \
		((u64)(GS_XYZ2)		<< 32)	| \
		((u64)(GIF_NOP)		<< 36);

// Textured Quad Primitive
/// Textured Quad Primitive GIFTAG
#define GIF_TAG_QUAD_TEXTURED(NLOOP)   \
		((u64)(NLOOP)		<< 0)	| \
		((u64)(1)		<< 15)	| \
		((u64)(0)		<< 46)	| \
		((u64)(0)		<< 47)	| \
		((u64)(1)		<< 58)	| \
		((u64)(12)		<< 60);
/// Textured Quad Primitive REGLIST
#define GIF_TAG_QUAD_TEXTURED_REGS(ctx)   \
		((u64)(GS_TEX0_1 + ctx)	<< 0)	| \
		((u64)(GS_PRIM)		<< 4)	| \
		((u64)(GS_RGBAQ)	<< 8)	| \
		((u64)(GS_UV)		<< 12)	| \
		((u64)(GS_XYZ2)		<< 16)	| \
		((u64)(GS_UV)		<< 20)	| \
		((u64)(GS_XYZ2)		<< 24)	| \
		((u64)(GS_UV)		<< 28)	| \
		((u64)(GS_XYZ2)		<< 32)	| \
		((u64)(GS_UV)		<< 36)	| \
		((u64)(GS_XYZ2)		<< 40)	| \
		((u64)(GIF_NOP)		<< 44);

/// Reserved GIF Tag. This is never sent, only used for sanity checks.
#define GIF_RESERVED	0x0B
/// GIF_TAG Address+Data Setting. (Used after GIF_TAG)
#define GIF_AD          0x0E
/// GIF No Operation (Used in GIF DMA Packets)
#define GIF_NOP         0x0F

/// Data cache prefetch (thx emoon!)
#define GSKIT_PREFETCH(address) asm __volatile__("pref 0, 0(%0)" : : "r" (address));

/// GS Reset Macro
#define GS_RESET() *GS_CSR = ((u64)(1)      << 9)

/// GIF Control Register
#define GIF_CTRL (volatile u32*)0x10003000

/// GIF Mode Settings Register
#define GIF_MODE (volatile u32*)0x10003010

/// GIF Status Register
#define GIF_STAT (volatile u32*)0x10003020

/// GIF Control Register Writing Routine
#define GIF_SETREG_CTRL(RST, PSE) \
	*GIF_CTRL =		  \
	((u32)(RST) << 0)	| \
	((u32)(PSE) << 3);

/// GIF Mode Settings Register Writing Routine
#define GIF_SETREG_MODE(M3R, IMT) \
	*GIF_MODE =		  \
	((u32)(M3R) << 0)	| \
	((u32)(IMT) << 2);

/// GIF Status Register Reading Macros
#define GIF_STAT_M3R	*GIF_STAT >> 0
#define GIF_STAT_M3P	*GIF_STAT >> 1
#define GIF_STAT_IMT	*GIF_STAT >> 2
#define GIF_STAT_PSE	*GIF_STAT >> 3
#define GIF_STAT_IP3	*GIF_STAT >> 5
#define GIF_STAT_P3Q	*GIF_STAT >> 6
#define GIF_STAT_P2Q	*GIF_STAT >> 7
#define GIF_STAT_P1Q	*GIF_STAT >> 8
#define GIF_STAT_OPH	*GIF_STAT >> 9
#define GIF_STAT_APATH	*GIF_STAT >> 10
#define GIF_STAT_DIR	*GIF_STAT >> 12
#define GIF_STAT_FQC	*GIF_STAT >> 24

/// GS CSR (GS System Status) Register
#define GS_CSR (volatile u64 *)0x12001000

/// CSR FINISH Register Access Routine
#define GS_CSR_FINISH ((*GS_CSR) >> 1)&1

/// CSR FINISH Register Writing Routine
#define GS_SETREG_CSR_FINISH(A) *GS_CSR = ((u64)(A) << 1)

/// GS BUSDIR (GS Bus Direction) Register
#define GS_BUSDIR ((volatile u64 *)(0x12001040))

/// GS BUSDIR Register Writing Routine
#define GS_SETREG_BUSDIR(A) *GS_BUSDIR = ((u64)(A))

/// GS IMR (GS Interrupt Mask) Register
#define GS_IMR ((volatile u64 *)(0x12001010))

/// GS SIGNALID (GS SIGNALID/LABELID) Register
#define GS_SIGNALID ((volatile u64 *)(0x12001080))

/// GS CSR (GS System Status) Register Access Macro
#define GS_SETREG_CSR(A,B,C,D,E,F,G,H,I,J,K,L) \
	(u64)(A & 0x00000001) <<  0 | (u64)(B & 0x00000001) <<  1 | \
	(u64)(C & 0x00000001) <<  2 | (u64)(D & 0x00000001) <<  3 | \
	(u64)(E & 0x00000001) <<  4 | (u64)(F & 0x00000001) <<  8 | \
	(u64)(G & 0x00000001) <<  9 | (u64)(H & 0x00000001) << 12 | \
	(u64)(I & 0x00000001) << 13 | (u64)(J & 0x00000003) << 14 | \
	(u64)(K & 0x000000FF) << 16 | (u64)(L & 0x000000FF) << 24

/// DMA Packet Header "GIF Tag" For Packets Sent To GIF
#define GIF_TAG(NLOOP,EOP,PRE,PRIM,FLG,NREG) \
                ((u64)(NLOOP)	<< 0)	| \
                ((u64)(EOP)	<< 15)	| \
                ((u64)(PRE)	<< 46)	| \
                ((u64)(PRIM)	<< 47)	| \
                ((u64)(FLG)	<< 58)	| \
                ((u64)(NREG)	<< 60);

/// Object Creation Macro for RGBAQ Color Values
#define GS_SETREG_RGBAQ(r, g, b, a, q) \
  ((u64)(r)        | ((u64)(g) << 8) | ((u64)(b) << 16) | \
  ((u64)(a) << 24) | ((u64)(q) << 32))

/// Object Creation Macro for RGBA Color Values
#define GS_SETREG_RGBA(r, g, b, a) \
        ((u64)(r)        | ((u64)(g) << 8) | ((u64)(b) << 16) | ((u64)(a) << 24))

/// GS Vertex Value (Without Drawing Kick) Object Creation Macro
#define GS_SETREG_XYZ3 GS_SETREG_XYZ
/// GS Vertex Value Object Creation Macro
#define GS_SETREG_XYZ2 GS_SETREG_XYZ
/// GS Vertex Value (Root Macro, XYZ2 and XYZ3 Call This) Object Creation Macro
#define GS_SETREG_XYZ(x, y, z) \
  ((u64)(x) | ((u64)(y) << 16) | ((u64)(z) << 32))

/// GS Vertex Value With Fog Coefficent (Without Drawing Kick) Object Creation Macro 
#define GS_SETREG_XYZF3 GS_SETREG_XYZF
/// GS Vertex Value With Fog Coefficent Object Creation Macro
#define GS_SETREG_XYZF2 GS_SETREG_XYZF
/// GS Vertex Value With Fog Coefficent (Root Macro, XYZF2 and XYZF3 Call This) Object Creation Macro 
#define GS_SETREG_XYZF(x, y, z, f) \
  ((u64)(x) | ((u64)(y) << 16) | ((u64)(z) << 32) | \
  ((u64)(f) << 56))

/// GS PCRTC (Merge Circuit) Registers
#define GS_PMODE	((volatile u64 *)(0x12000000))
#define GS_SMODE1	((volatile u64 *)(0x12000010))
#define GS_SMODE2	((volatile u64 *)(0x12000020))
#define GS_SRFSH	((volatile u64 *)(0x12000030))
#define GS_SYNCH1	((volatile u64 *)(0x12000040))
#define GS_SYNCH2	((volatile u64 *)(0x12000050))
#define GS_SYNCV	((volatile u64 *)(0x12000060))
#define GS_EXTBUF	((volatile u64 *)(0x120000B0))
#define GS_EXTDATA	((volatile u64 *)(0x120000C0))
#define GS_EXTWRITE	((volatile u64 *)(0x120000D0))

/// GS Framebuffer Register (Output Circuit 1)
#define GS_DISPFB1	((volatile u64 *)(0x12000070))

/// GS Framebuffer Register (Output Circuit 2)
#define GS_DISPFB2	((volatile u64 *)(0x12000090))

/// GS PCRTC (Merge Circuit) Register Access Macro
#define GS_SET_PMODE(EN1,EN2,MMOD,AMOD,SLBG,ALP) \
        *GS_PMODE = \
        ((u64)(EN1)     << 0)   | \
        ((u64)(EN2)     << 1)   | \
        ((u64)(001)     << 2)   | \
        ((u64)(MMOD)    << 5)   | \
        ((u64)(AMOD)	<< 6)	| \
        ((u64)(SLBG)	<< 7)	| \
        ((u64)(ALP)     << 8)

/// GS PCRTC (Merge Circuit) Register Access Macro (Extended for EXTBUF External Digital In)
#define GS_SET_PMODE_EXT(EN1,EN2,CRTMD,MMOD,AMOD,SLBG,ALP, NFLD, EXVWINS, EXVWINE, EVSYNCMD) \
        *GS_PMODE = \
        ((u64)(EN1)     << 0)   | \
        ((u64)(EN2)     << 1)   | \
        ((u64)(CRTMD)   << 2)   | \
        ((u64)(MMOD)    << 5)   | \
        ((u64)(AMOD)	<< 6)	| \
        ((u64)(SLBG)	<< 7)	| \
        ((u64)(ALP)     << 8)	| \
        ((u64)(NFLD)	<< 16)	| \
        ((u64)(EXVWINS)	<< 32)	| \
        ((u64)(EXVWINE)	<< 42)	| \
        ((u64)(EVSYNCMD) << 52)

/// GS PCRTC (Merge Circuit) EXTBUF Register Access Macro
#define GS_SET_EXTBUF(EXBP,EXBW,FBIN,WFFMD,EMODA,EMODC,WDX,WDY) \
        *GS_EXTBUF = \
	((u64)(EXBP)	<< 0)	| \
	((u64)(EXBW)	<< 14)	| \
	((u64)(FBIN)	<< 20)	| \
	((u64)(WFFMD)	<< 22)	| \
	((u64)(EMODA)	<< 23)	| \
	((u64)(EMODC)	<< 25)	| \
	((u64)(WDX)	<< 32)	| \
	((u64)(WDY)	<< 43)

/// GS PCRTC (Merge Circuit) EXTDATA Register Access Macro
#define GS_SET_EXTDATA(SX,SY,SMPH,SMPV,WW,WH) \
        *GS_EXTDATA = \
	((u64)(SX)	<< 0)	| \
	((u64)(SY)	<< 12)	| \
	((u64)(SMPH)	<< 23)	| \
	((u64)(SMPV)	<< 27)	| \
	((u64)(WW)	<< 32)	| \
	((u64)(WH)	<< 44)

/// GS PCRTC (Merge Circuit) EXTWRITE Register Access Macro
#define GS_SET_EXTWRITE(WRITE) \
        *GS_EXTWRITE = \
	((u64)(WRITE)	<< 0)

/// GS Framebuffer Register Access Macro (Output Circuit 1)
#define GS_SET_DISPFB1(FBP,FBW,PSM,DBX,DBY) \
        *GS_DISPFB1 = \
        ((u64)(FBP)     << 0)   | \
        ((u64)(FBW)     << 9)   | \
        ((u64)(PSM)     << 15)  | \
        ((u64)(DBX)     << 32)  | \
        ((u64)(DBY)     << 43)

/// GS Framebuffer Register Access Macro (Output Circuit 2)
#define GS_SET_DISPFB2(FBP,FBW,PSM,DBX,DBY) \
        *GS_DISPFB2 = \
        ((u64)(FBP)     << 0)   | \
        ((u64)(FBW)     << 9)   | \
        ((u64)(PSM)     << 15)  | \
        ((u64)(DBX)     << 32)  | \
        ((u64)(DBY)     << 43)

/// GS Display Settings Register (Output Circuit 1)
#define GS_DISPLAY1        ((volatile u64 *)(0x12000080))

/// GS Display Settings Register Access Macro (Output Circuit 1)
#define GS_SET_DISPLAY1(DX,DY,MAGH,MAGV,DW,DH) \
        *GS_DISPLAY1 = \
        ((u64)(DX)      << 0)   | \
        ((u64)(DY)      << 12)  | \
        ((u64)(MAGH)    << 23)  | \
        ((u64)(MAGV)    << 27)  | \
        ((u64)(DW)      << 32)  | \
        ((u64)(DH)      << 44)

/// GS Display Settings Register (Output Circuit 2)
#define GS_DISPLAY2        ((volatile u64 *)(0x120000a0))

/// GS Display Settings Register Access Macro (Output Circuit 2)
#define GS_SET_DISPLAY2(DX,DY,MAGH,MAGV,DW,DH) \
        *GS_DISPLAY2 = \
        ((u64)(DX)      << 0)   | \
        ((u64)(DY)      << 12)  | \
        ((u64)(MAGH)    << 23)  | \
        ((u64)(MAGV)    << 27)  | \
        ((u64)(DW)      << 32)  | \
        ((u64)(DH)      << 44)

/// GS Background Color Register
#define GS_BGCOLOR         ((volatile u64 *)(0x120000e0))

/// GS Background Color Register Access Macro
#define GS_SET_BGCOLOR(R,G,B) \
        *GS_BGCOLOR = \
        ((u64)(R)       << 0)           | \
        ((u64)(G)       << 8)           | \
        ((u64)(B)       << 16)

/// See GS_SETREG_XYOFFSET
#define GS_SETREG_XYOFFSET_1  GS_SETREG_XYOFFSET
/// See GS_SETREG_XYOFFSET
#define GS_SETREG_XYOFFSET_2  GS_SETREG_XYOFFSET
/// GS XYOffset Packing Macro
#define GS_SETREG_XYOFFSET(ofx, ofy) ((u64)(ofx) | ((u64)(ofy) << 32))

/// See GS_SETREG_ZBUF
#define GS_SETREG_ZBUF_1  GS_SETREG_ZBUF
/// See GS_SETREG_ZBUF
#define GS_SETREG_ZBUF_2  GS_SETREG_ZBUF
/// GS ZBUF Packing Macro
#define GS_SETREG_ZBUF(zbp, psm, zmsk) \
  ((u64)(zbp) | ((u64)(psm) << 24) | \
  ((u64)(zmsk) << 32))

/// See GS_SETREG_TEST
#define GS_SETREG_TEST_1 GS_SETREG_TEST
/// See GS_SETREG_TEST
#define GS_SETREG_TEST_2 GS_SETREG_TEST
/// GS TEST Packing Macro
#define GS_SETREG_TEST(ate, atst, aref, afail, date, datm, zte, ztst) \
  ((u64)(ate)         | ((u64)(atst) << 1) | \
  ((u64)(aref) << 4)  | ((u64)(afail) << 12) | \
  ((u64)(date) << 14) | ((u64)(datm) << 15) | \
  ((u64)(zte) << 16)  | ((u64)(ztst) << 17))

/// GS COLCLAMP Packing Macro
#define GS_SETREG_COLCLAMP(clamp) ((u64)(clamp))

/// See GS_SETREG_FRAME
#define GS_SETREG_FRAME_1 GS_SETREG_FRAME
/// See GS_SETREG_FRAME
#define GS_SETREG_FRAME_2 GS_SETREG_FRAME
/// GS FRAME Packing Macro
#define GS_SETREG_FRAME(fbp, fbw, psm, fbmask) \
  ((u64)(fbp)        | ((u64)(fbw) << 16) | \
  ((u64)(psm) << 24) | ((u64)(fbmask) << 32))  

/// See GS_SETREG_SCISSOR
#define GS_SETREG_SCISSOR_1 GS_SETREG_SCISSOR
/// See GS_SETREG_SCISSOR
#define GS_SETREG_SCISSOR_2 GS_SETREG_SCISSOR
/// GS SCISSOR Packing Macro
#define GS_SETREG_SCISSOR(scax0, scax1, scay0, scay1) \
  ((u64)(scax0)        | ((u64)(scax1) << 16) | \
  ((u64)(scay0) << 32) | ((u64)(scay1) << 48))

/// See GS_SETREG_CLAMP
#define GS_SETREG_CLAMP_1 GS_SET_CLAMP
/// See GS_SETREG_CLAMP
#define GS_SETREG_CLAMP_2 GS_SET_CLAMP
/// GS CLAMP Packing Macro
#define GS_SETREG_CLAMP(wms, wmt, minu, maxu, minv, maxv) \
  ((u64)(wms)         | ((u64)(wmt) << 2) | \
  ((u64)(minu) << 4)  | ((u64)(maxu) << 14) | \
  ((u64)(minv) << 24) | ((u64)(maxv) << 34))

/// GS ALPHA Packing Macro
#define GS_SETREG_ALPHA(A, B, C, D, FIX) \
	(((u64)(A)	<< 0)	| \
	((u64)(B)	<< 2)	| \
	((u64)(C)	<< 4)	| \
	((u64)(D)	<< 6)	| \
	((u64)(FIX)	<< 32))

/// Background Color Structure
struct gsBGColor
{
	u8 Red;		///< Red Value
	u8 Green;	///< Green Value
	u8 Blue;	///< Blue Value	
};
typedef struct gsBGColor GSBGCOLOR;

/// TEST Register Settings Structure
struct gsTest
{
	u8 ATE;		///< Alpha Test Enable
	u8 ATST;	///< Alpha Test Method
	u8 AREF;	///< Alpha Reference Value
	u8 AFAIL;	///< Alpha Failure Action
	u8 DATE;	///< Destination Alpha Enable
	u8 DATM;	///< Destination Alpha Mode
	u8 ZTE;		///< Depth Test Enable (DO NOT USE THIS - IT IS BUGGED IN PARTICULAR GS REV - SET TO 1 _ALWAYS_)
	u8 ZTST;	///< Depth Test Method
};
typedef struct gsTest GSTEST;

/// CLAMP Register Settings Structure
struct gsClamp
{
	u8 WMS;		///< Wrap Mode Horizontal
	u8 WMT;		///< Wrap Mode Vertical
	int MINU;	///< Minimum Horizontal Clamp Value
	int MAXU;	///< Maximum Horizontal Clamp Value
	int MINV;	///< Minimum Vertical Clamp Value
	int MAXV;	///< Maximum Vertical Clamp Value
};
typedef struct gsClamp GSCLAMP;

/// gsQueue Structure
/// This is the structure for gsKit draw queues.
///
/// By default, gsKit creates two of these for you.
/// It creates Os_Queue (Oneshot Objects), and Per_Queue. (Persistent Objects)
///
/// You can create your own gsQueue objects and load them with prepacked GIF/VIF chains!
struct gsQueue
{
	void *pool[2] __attribute__ ((aligned (64)));	///< Pool Base Address (Two of these, since we doublebuffer)
	void *pool_cur;					///< Current Pool Address (Offset from Base)
	void *pool_max[2];				///< Pool Maximum Address (Used for bounds checking)

	int last_type;					///< Last type of object in this draw queue
	void *last_tag;					///< Pointer to the GIF_TAG of the last object in this draw queue

	int same_obj;					///< Number of times in a row the same REGLIST enabled object has been added (Used for NLOOP fixup later)

	void *dma_tag;					///< Pointer to last DMA_TAG
	u32 tag_size;					///< Quadwords in current DMA_TAG

	u8 mode;					///< Mode of the draw queue (NOT USED RIGHT NOW)
	int dbuf;					///< Which of the pool[2] buffers we are using
};
typedef struct gsQueue GSQUEUE;

/// gsGlobal Structure
/// This is the core structure for gsKit operation.
///
/// All relevant data to GS operation including framebuffer pointers,
/// display mode, dimensions, offsets, and GS register values are maintained
/// here.
///
/// Most gsKit functions will require the pointer of gsGlobal as their first
/// parameter.
///
/// To create a gsGlobal pointer it is recommended you use the gsKit_init_global
/// routine.
///
/// Example: GSGLOBAL *gsGlobal = gsKit_init_global(GS_MODE_NTSC);
///
/// This will create, allocate, and preload gsGlobal with all default values
/// and specific values for the NTSC display mode.
struct gsGlobal
{
	s16 Mode;		///< Display Mode
	s16 Interlace;		///< Interlace (On/Off)
	s16 Field;		///< Field / Frame
	u8 Setup;		///< Setup (Is set to 1 after screen init, see gsKit_init() for more info.)
	u32 CurrentPointer;	///< Current VRAM Pointer
	u8 DoubleBuffering;	///< Enable/Disable Double Buffering
	u8 ZBuffering;		///< Enable/Disable Z Buffering
	u32 ScreenBuffer[2];	///< Screenbuffer Pointer Array
	u32 ZBuffer;		///< ZBuffer Pointer
	u8 DoSubOffset;		///< Do Subpixel Offset
	u8 EvenOrOdd;		///< Is ((GSREG*)CSR)->FIELD (Used for Interlace Correction)
	u8 DrawOrder;		///< Drawing Order (GS_PER_OS/GS_OS_PER) (GS_PER_OS = Persitant objects always drawn first)
	u8 FirstFrame;		///< Have we executed a frame yet?
	int ActiveBuffer;	///< Active Framebuffer
	int Width;		///< Framebuffer Width
	int Height;		///< Framebuffer Height
	int Aspect;		///< Framebuffer Aspect Ratio (Not Currently Used)
	int OffsetX;		///< X Window Offset
	int OffsetY;		///< Y Window Offset
	int StartX;		///< X Starting Coordinate (Used for Placement Correction)
	int StartY;		///< Y Starting Coordinate (Used for Placement Correction)
	int MagX;		///< X Magnification Value
	int MagY;		///< Y Magnification Value (Always 0!)
	GSBGCOLOR *BGColor;	///< Background Color Structure Pointer
	GSTEST *Test;		///< TEST Register Value Structure Pointer
	GSCLAMP *Clamp;		///< CLAMP Register Value Structure Pointer
	GSQUEUE *CurQueue; 	///< Draw Queue (Current)
	GSQUEUE *Per_Queue; 	///< Draw Queue (Persistent)
	GSQUEUE *Os_Queue; 	///< Draw Queue (Oneshot)
	int Os_AllocSize;	///< Oneshot Drawbuffer Size (Per Buffer)
	int Per_AllocSize;	///< Persistent Drawbuffer Size (Per Buffer)
	void *dma_misc __attribute__ ((aligned (64)));	///< Misc 512 byte DMA Transfer Buffer (so we don't malloc at runtime)
	int PSM;		///< Pixel Storage Method (Color Mode)
	int PSMZ;		///< ZBuffer Pixel Storage Method
	int PrimContext;	///< Primitive Context
	int PrimFogEnable;	///< Primitive Fog Enable
	int PrimAAEnable;	///< Primitive AntiAlaising Enable
	int PrimAlphaEnable;	///< Primitive Alpha Blending Enable
	u64 PrimAlpha;		///< Primitive Alpha Value
	u8  PABE;		///< Choose if to do Alpha Blend on a Per-Pixel Basis
};
typedef struct gsGlobal GSGLOBAL;

/// gsKit Texture Structure
/// This structure holds all relevant data for any
/// given texture object, regardless of original format or type.
struct gsTexture
{
	u32     Width;		///< Width of the Texture
	u32     Height;		///< Height of the Texture
	u8	PSM;		///< Pixel Storage Method (Color Format)
	u8	ClutPSM;	///< CLUT Pixel Storage Method (Color Format)
	u32	TBW;		///< Texture Base Width
	u32    *Mem;		///< EE Memory Pointer
	u32    *Clut;		///< EE CLUT Memory Pointer
	u32     Vram;		///< GS VRAM Memory Pointer
	u32     VramClut;	///< GS VRAM CLUT Memory Pointer
	u32     Filter;		///< NEAREST or LINEAR
	u8	Delayed;	///< Delay Texture Upload To VRAM
};
typedef struct gsTexture GSTEXTURE;

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
typedef struct gsKit_fontm_header GSFONTM;

/// gsKit Font Structure
/// This structure holds all relevant data for any
/// given font object, regardless of original format or type.
struct gsFont
{
	char *Path;		///< Path (string) to the Font File
	char *Path_BMP;		///< Path (string) to the BMP Glyph File
	char *Path_DAT;		///< Path (string) to the Glyph DAT File
	u8 Type;		///< Font Type
	u8 *RawData;		///< Raw File Data
	int RawSize;		///< Raw File Datasize
	GSTEXTURE *Texture;	///< Font Texture Object
	GSFONTM FontM_Header;	///< FONTM Header
	u32 FontM_Vram[2];	///< FONTM VRAM Allocation (Double Buffered)
	u32 FontM_VramIdx;	///< FONTM Current Double Buffer Index
	u32 FontM_LastPage;	///< FONTM Last Uploaded Texture Page
	u8 FontM_Align;		///< FONTM Line Alignment
	float FontM_Spacing;	///< FONTM Glyph Spacing
	void *TexBase;		///< Glyphs Texture Base
	u32 CharWidth;		///< Character Width
	u32 CharHeight;		///< Character Height
	u32 HChars;		///< Character Rows
	u32 VChars;		///< Character Columns
	u8 *Additional;		///< Additional Font Data
};
typedef struct gsFont GSFONT;

/// Alternative Access Method to the GS CSR Register
struct gsRegisters {
 u64 SIGNAL:      1 __attribute__((packed)); /* ro */
 u64 FINISH:      1 __attribute__((packed)); /* ro */
 u64 HSINT:       1 __attribute__((packed)); /* ro */
 u64 VSINT:       1 __attribute__((packed)); /* ro */
 u64 reserved04:  3 __attribute__((packed)); /* ro */
 u64 pad07:       1 __attribute__((packed));
 u64 FLUSH:       1 __attribute__((packed)); /* rw */
 u64 RESET:       1 __attribute__((packed)); /* N/A */
 u64 pad10:       2 __attribute__((packed));
 u64 NFIELD:      1 __attribute__((packed)); /* ro */
 u64 FIELD:       1 __attribute__((packed)); /* ro */
 u64 FIFO:        2 __attribute__((packed)); /* ro */
 u64 REV:         8 __attribute__((packed)); /* ro */
 u64 ID:          8 __attribute__((packed)); /* ro */
 u64 pad32:      32 __attribute__((packed));
};
typedef struct gsRegisters GSREG __attribute__((packed));

#ifdef __cplusplus
extern "C" {
#endif

/// Detect signal (returns GS_MODE_NTSC or GS_MODE_PAL)
int gsKit_detect_signal();
/// Initialize Screen and GS Registers
void gsKit_init_screen(GSGLOBAL *gsGlobal);
/// Initialize gsGlobal (With Specified Sizes (In Bytes) for the Persistent and Oneshot drawbuffers)
GSGLOBAL *gsKit_init_global_custom(u8 mode, int Os_AllocSize, int Per_AllocSize);
/// Initialize Font Object
GSFONT *gsKit_init_font(u8 type, char *path);
/// Initialize Font Object (From Memory)
GSFONT *gsKit_init_font_raw(u8 type, u8 *data, int size);


#define gsKit_init_global(mode) \
		gsKit_init_global_custom(mode, GS_RENDER_QUEUE_OS_POOLSIZE, GS_RENDER_QUEUE_PER_POOLSIZE);

#ifdef __cplusplus
}
#endif

#endif /* __GSINIT_H__ */
