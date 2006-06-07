//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsMisc.h - Header for gsMisc.c
//

#ifndef __GSMISC_H__
#define __GSMISC_H__

#include "gsKit.h"

#ifdef __cplusplus
extern "C" {
#endif

void gsKit_setup_tbw(GSTEXTURE *Texture);
void gsKit_vram_dump(GSGLOBAL *gsGlobal, char *Path, u32 StartAddr, u32 EndAddr);

#ifdef __cplusplus
}
#endif

#endif /* __GSMISC_H__ */
