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

#ifndef __GSCORE_H__
#define __GSCORE_H__

#include "gsKit.h"

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

#define GIF_AD          0x0e
#define GIF_NOP         0x0f

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


#endif /* __GSCORE_H__ */
