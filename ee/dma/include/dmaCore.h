//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// dmaCore.h - Header for dmaCore.c
//

#ifndef __DMACORE_H__
#define __DMACORE_H__

#include "dmaKit.h"

#define D_CHAN_VIF1 0x1
#define D_CHAN_GIF 0x2
#define D_CHAN_FROMIPU 0x3
#define D_CHAN_SIF0 0x5
#define D_CHAN_SIF1 0x6
#define D_CHAN_FROMSPR 0x8

void dmaKit_Wait(unsigned int channel);

#endif /* __DMACORE_H__ */
