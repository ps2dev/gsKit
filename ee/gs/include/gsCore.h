//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsCore.h - Header for gsCore.c
//
// Parts taken from emoon's BreakPoint Demo Library
//

#ifndef __GSCORE_H__
#define __GSCORE_H__

#include "gsKit.h"

#define GS_ASPECT_4_3 0x0
#define GS_ASPECT_16_9 0x1

#define GS_AXIS_X 0x00
#define GS_AXIS_Y 0x01


#define GS_CMODE_REPEAT 0x00
#define GS_CMODE_CLAMP 0x01
#define GS_CMODE_REGION_CLAMP 0x02
#define GS_CMODE_REGION_REPEAT 0x03
#define GS_CMODE_INIT 0xFF

#define GS_ZTEST_OFF 0x00
#define GS_ZTEST_ON 0x01
#define GS_ATEST_OFF 0x00
#define GS_ATEST_ON 0x01
#define GS_D_ATEST_OFF 0x00
#define GS_D_ATEST_ON 0x01
#define GS_ZTEST_INIT 0xFF

#define GSU_REG_DISPLAY1       (volatile u64 *)0x12000080
#define GSU_REG_DISPLAY2       (volatile u64 *)0x120000A0

#define GS_PRIM       0x00
#define GS_RGBAQ      0x01
#define GS_ST         0x02
#define GS_UV         0x03
#define GS_XYZF2      0x04
#define GS_XYZ2       0x05
#define GS_TEX0_1     0x06
#define GS_TEX0_2     0x07
#define GS_CLAMP_1    0x08
#define GS_CLAMP_2    0x09
#define GS_FOG        0x0a
#define GS_XYZF3      0x0c
#define GS_XYZ3       0x0d
#define GS_TEX1_1     0x14
#define GS_TEX1_2     0x15
#define GS_TEX2_1     0x16
#define GS_TEX2_2     0x17
#define GS_XYOFFSET_1 0x18
#define GS_XYOFFSET_2 0x19
#define GS_PRMODECONT 0x1a
#define GS_PRMODE     0x1b
#define GS_TEXCLUT    0x1c
#define GS_SCANMSK    0x22
#define GS_MIPTBP1_1  0x34
#define GS_MIPTBP1_2  0x35
#define GS_MIPTBP2_1  0x36
#define GS_MIPTBP2_2  0x37
#define GS_TEXA       0x3b
#define GS_FOGCOL     0x3d
#define GS_TEXFLUSH   0x3f
#define GS_SCISSOR_1  0x40
#define GS_SCISSOR_2  0x41
#define GS_ALPHA_1    0x42
#define GS_ALPHA_2    0x43
#define GS_DIMX       0x44
#define GS_DTHE       0x45
#define GS_COLCLAMP   0x46
#define GS_TEST_1     0x47
#define GS_TEST_2     0x48
#define GS_PABE       0x49
#define GS_FBA_1      0x4a
#define GS_FBA_2      0x4b
#define GS_FRAME_1    0x4c
#define GS_FRAME_2    0x4d
#define GS_ZBUF_1     0x4e
#define GS_ZBUF_2     0x4f
#define GS_BITBLTBUF  0x50
#define GS_TRXPOS     0x51
#define GS_TRXREG     0x52
#define GS_TRXDIR     0x53
#define GS_HWREG      0x54
#define GS_SIGNAL     0x60
#define GS_FINISH     0x61
#define GS_LABEL      0x62
#define GS_NOP        0x7f

#define GS_PRIM_PRIM_POINT    0
#define GS_PRIM_PRIM_LINE   1
#define GS_PRIM_PRIM_LINESTRIP  2
#define GS_PRIM_PRIM_TRIANGLE 3
#define GS_PRIM_PRIM_TRISTRIP 4
#define GS_PRIM_PRIM_TRIFAN   5
#define GS_PRIM_PRIM_SPRITE   6
#define GS_PRIM_IIP_FLAT    0
#define GS_PRIM_IIP_GOURAUD   1
#define GS_PRIM_TME_OFF   0
#define GS_PRIM_TME_ON    1
#define GS_PRIM_FGE_OFF   0
#define GS_PRIM_FGE_ON    1
#define GS_PRIM_ABE_OFF   0
#define GS_PRIM_ABE_ON    1
#define GS_PRIM_AA1_OFF   0
#define GS_PRIM_AA1_ON    1
#define GS_PRIM_FST_STQ   0
#define GS_PRIM_FST_UV    1
#define GS_PRIM_CTXT_CONTEXT1 0
#define GS_PRIM_CTXT_CONTEXT2 1
#define GS_PRIM_FIX_NOFIXDDA  0
#define GS_PRIM_FIX_FIXDDA    1

#define GIF_AD          0x0E
#define GIF_NOP         0x0F

#define GS_VRAM_BLOCKSIZE 8192
#define GS_VRAM_BLOCKSIZEW 64
#define GS_VRAM_BLOCKSIZEH 32

#define CSR ((volatile u64 *)(0x12001000))
#define GS_RESET() *CSR = ((u64)(1)      << 9)
#define GSSREG_CSR    0x40
#define GS_CSR_FLUSH    1
#define GS_CSR_FIELD_EVEN   0
#define GS_CSR_FIELD_ODD    1
#define GS_CSR_FIFO_HALFFULL  0
#define GS_CSR_FIFO_EMPTY   1
#define GS_CSR_FIFO_ALMOSTFULL  2

#define GSU_REG_CSR            (volatile u64 *)0x12001000      // System Status

#define GSU_SET_CSR(A,B,C,D,E,F,G,H,I,J,K,L) \
	(u64)(A & 0x00000001) <<  0 | (u64)(B & 0x00000001) <<  1 | \
	(u64)(C & 0x00000001) <<  2 | (u64)(D & 0x00000001) <<  3 | \
	(u64)(E & 0x00000001) <<  4 | (u64)(F & 0x00000001) <<  8 | \
	(u64)(G & 0x00000001) <<  9 | (u64)(H & 0x00000001) << 12 | \
	(u64)(I & 0x00000001) << 13 | (u64)(J & 0x00000003) << 14 | \
	(u64)(K & 0x000000FF) << 16 | (u64)(L & 0x000000FF) << 24

#define GIF_TAG(NLOOP,EOP,PRE,PRIM,FLG,NREG) \
                ((u64)(NLOOP)<< 0)              | \
                ((u64)(EOP)     << 15)          | \
                ((u64)(PRE)     << 46)          | \
                ((u64)(PRIM)    << 47)  | \
                ((u64)(FLG)     << 58)          | \
                ((u64)(NREG)    << 60);

#define GS_SETREG_RGBAQ(r, g, b, a, q) \
  ((u64)(r)        | ((u64)(g) << 8) | ((u64)(b) << 16) | \
  ((u64)(a) << 24) | ((u64)(q) << 32))

#define GS_SETREG_RGBA(r, g, b, a) \
        ((u64)(r)        | ((u64)(g) << 8) | ((u64)(b) << 16) | ((u64)(a) << 24))

#define GS_SETREG_XYZ3 GS_SETREG_XYZ
#define GS_SETREG_XYZ2 GS_SETREG_XYZ
#define GS_SETREG_XYZ(x, y, z) \
  ((u64)(x) | ((u64)(y) << 16) | ((u64)(z) << 32))

#define GS_SETREG_XYZF3 GS_SETREG_XYZF
#define GS_SETREG_XYZF2 GS_SETREG_XYZF
#define GS_SETREG_XYZF(x, y, z, f) \
  ((u64)(x) | ((u64)(y) << 16) | ((u64)(z) << 32) | \
  ((u64)(f) << 56))

struct gsTexture
{
        u32     Width;
        u32     Height;
        u32     PSM;
        void    *Mem;
        void    *Clut;
        u32     Vram;
        u32     VramClut;
};
typedef struct gsTexture GSTEXTURE;

struct gsFont
{
        char *Path;
        u8 Type;
        GSTEXTURE Texture;
        int CharWidth;
};
typedef struct gsFont GSFONT;

u32 gsKit_vram_alloc(GSGLOBAL *gsGlobal, int size);
void gsKit_sync_flip(GSGLOBAL *gsGlobal);
void gsKit_setactive(GSGLOBAL *gsGlobal);
void gsKit_vsync(void);
void gsKit_clear(GSGLOBAL *gsGlobal, u64 Color);
void gsKit_set_test(GSGLOBAL *gsGlobal, u8 Preset);
void gsKit_set_clamp(GSGLOBAL *gsGlobal, u8 Preset);

#endif /* __GSCORE_H__ */
