//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsPrimitive.h - Header for gsPrimitive.c
//
// Parts taken from emoon's BreakPoint Demo Library
//

#ifndef __GSPRIMITIVE_H__
#define __GSPRIMITIVE_H__

#include "gsKit.h"

#define GS_SETREG_PRIM(prim, iip, tme, fge, abe, aa1, fst, ctxt, fix) \
  ((u64)(prim)      | ((u64)(iip) << 3)  | ((u64)(tme) << 4) | \
  ((u64)(fge) << 5) | ((u64)(abe) << 6)  | ((u64)(aa1) << 7) | \
  ((u64)(fst) << 8) | ((u64)(ctxt) << 9) | ((u64)(fix) << 10))

#define GS_SETREG_PRMODE(iip, tme, fge, abe, aa1, fst, ctxt, fix) \
  (((u64)(iip) << 3) | ((u64)(tme) << 4)  | \
   ((u64)(fge) << 5) | ((u64)(abe) << 6)  | ((u64)(aa1) << 7) | \
   ((u64)(fst) << 8) | ((u64)(ctxt) << 9) | ((u64)(fix) << 10))

#define GS_SETREG_PRMODECONT(ac) ((u64)(ac))

int gsKit_prim_sprite(GSGLOBAL gsGlobal, int x1, int y1, int x2, int y2, int z, u64 color);

#endif /* __GSPRIMITIVE_H__ */
