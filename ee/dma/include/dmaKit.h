//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// dmaKit.h - Master header for dmaKit. Include _ONLY_THIS_HEADER_
//           for gsKit. (Do NOT include dmacore.h, etc)
//
// Parts taken from mrbrown's dmac tutorial.
//

#ifndef __DMAKIT_H__
#define __DMAKIT_H__

#include <tamtypes.h>

#include "dmaCore.h"
#include "dmaInit.h"

#define DMA_REG_CTRL		(volatile u32 *)0x1000E000
#define DMA_REG_STAT		(volatile u32 *)0x1000E010
#define DMA_REG_PCR		(volatile u32 *)0x1000E020
#define DMA_REG_SQWC		(volatile u32 *)0x1000E030
#define DMA_REG_RBSR		(volatile u32 *)0x1000E040
#define DMA_REG_RBOR		(volatile u32 *)0x1000E050

#endif /* __DMAKIT_H__ */
