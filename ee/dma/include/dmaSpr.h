//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// dmaSpr.h - Header for dmaSpr.c
// Parts taken from emoon's BreakPoint Demo Library
//

#ifndef __DMASPR_H__
#define __DMASPR_H__

#include "dmaKit.h"

#define SPR_START 0x70000000

#ifdef __cplusplus
extern "C" {
#endif

/// DMA Scratchpad Chain Begin
void *dmaKit_spr_begin(void);

/// DMA Scratchpad Chain End
int dmaKit_spr_end(void *data, void *StorePtr);

/// DMA Scratchpad Allocation
void *dmaKit_spr_alloc(int size);

#ifdef __cplusplus
}
#endif

#endif /* __DMASPR_H__ */
