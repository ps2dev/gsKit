//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2017 - Rick "Maximus32" Gaiser <rgaiser@gmail.com>
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsHires.h - Header for gsHires.c
//

#ifndef __GSHIRES_H__
#define __GSHIRES_H__


#include "gsKit.h"


#ifdef __cplusplus
extern "C" {
#endif


/// Wait for VSync
void gsKit_hires_sync(GSGLOBAL *gsGlobal);

/// Flips Draw Queue
void gsKit_hires_flip(GSGLOBAL *gsGlobal);

/// Converts PSM and interlacing for use as background image
void gsKit_hires_prepare_bg(GSGLOBAL *gsGlobal, GSTEXTURE * tex);

/// Set background texture, must be same size and PSM as display
int  gsKit_hires_set_bg(GSGLOBAL *gsGlobal, GSTEXTURE * tex);

/// Initialize Screen and GS Registers, for multi pass high resolution mode
void gsKit_hires_init_screen(GSGLOBAL *gsGlobal, int passCount);

/// Initialize gsGlobal
GSGLOBAL *gsKit_hires_init_global();

/// De-initialize gsGlobal
void gsKit_hires_deinit_global(GSGLOBAL *gsGlobal);


#ifdef __cplusplus
}
#endif


#endif /* __GSHIRES_H__ */
