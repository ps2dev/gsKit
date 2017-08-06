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

#ifndef __GSTEXMANAGER_H__
#define __GSTEXMANAGER_H__


#include "gsKit.h"


#ifdef __cplusplus
extern "C" {
#endif


enum ETransferMode {
	ETM_INLINE = 0,
	ETM_DIRECT
};


/// Initialize the texture manager
void gsKit_TexManager_init(GSGLOBAL * gsGlobal);

/// Set the texture transfer mode (inline or direct)
void gsKit_TexManager_setmode(GSGLOBAL * gsGlobal, enum ETransferMode mode);

/// Bind a texture to VRAM, will automatically transfer the texture.
unsigned int gsKit_TexManager_bind(GSGLOBAL * gsGlobal, GSTEXTURE * tex);

/// Invalidate a texture, will automatically transfer the texture on next bind call.
void gsKit_TexManager_invalidate(GSGLOBAL * gsGlobal, GSTEXTURE * tex);

/// Free the texture, this is mainly a performance optimization.
/// The texture will be automatically freed if not used.
void gsKit_TexManager_free(GSGLOBAL * gsGlobal, GSTEXTURE * tex);

/// When starting a next frame (on vsync/swap), call this function.
/// It updates texture usage statistics.
void gsKit_TexManager_nextFrame(GSGLOBAL * gsGlobal);


#ifdef __cplusplus
};
#endif


#endif /* __GSTEXMANAGER_H__ */
