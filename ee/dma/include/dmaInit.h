//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// dmaInit.h - Header for dmaInit.c
//
// Portions taken from ooPo's tutorial at:
// http://www.oopo.net/consoledev/files/ps2-using-the-dmac.txt
//

#ifndef __DMAINIT_H__
#define __DMAINIT_H__

#include "dmaKit.h"

#define D_CTRL_RELE_OFF 0x0
#define D_CTRL_RELE_ON 0x1

#define D_CTRL_MFD_OFF 0x0
#define D_CTRL_MFD_RES 0x1
#define D_CTRL_MFD_VIF 0x2
#define D_CTRL_MFD_GIF 0x3

#define D_CTRL_STS_UNSPEC 0x0
#define D_CTRL_STS_SIF 0x1
#define D_CTRL_STS_SPR 0x2
#define D_CTRL_STS_IPU 0x3

#define D_CTRL_STD_OFF 0x0
#define D_CTRL_STD_VIF 0x1
#define D_CTRL_STD_GIF 0x2
#define D_CTRL_STD_SIF 0x3

#define D_CTRL_RCYC_8 0x0
#define D_CTRL_RCYC_16 0x1
#define D_CTRL_RCYC_32 0x2
#define D_CTRL_RCYC_64 0x3
#define D_CTRL_RCYC_128 0x4
#define D_CTRL_RCYC_256 0x5

#define DMA_REG_CTRL	(volatile u32 *)0x1000E000
#define DMA_REG_STAT	(volatile u32 *)0x1000E010
#define DMA_REG_PCR	(volatile u32 *)0x1000E020
#define DMA_REG_SQWC	(volatile u32 *)0x1000E030
#define DMA_REG_RBSR	(volatile u32 *)0x1000E040
#define DMA_REG_RBOR	(volatile u32 *)0x1000E050

#define DMA_CHANNEL_VIF0	0x0
#define DMA_CHANNEL_VIF1	0x1
#define DMA_CHANNEL_GIF		0x2
#define DMA_CHANNEL_FROMIPU	0x3
#define DMA_CHANNEL_TOIPU	0x4
#define DMA_CHANNEL_SIF0	0x5 
#define DMA_CHANNEL_SIF1	0x6
#define DMA_CHANNEL_SIF2	0x7
#define DMA_CHANNEL_FROMSPR	0x8
#define DMA_CHANNEL_TOSPR	0x9

#define DMA_SET_CTRL(A,B,C,D,E,F) \
    (u32)(A & 0x00000001) <<  0 | (u32)(B & 0x00000001) <<  1 | \
    (u32)(C & 0x00000003) <<  2 | (u32)(D & 0x00000003) <<  4 | \
    (u32)(E & 0x00000003) <<  6 | (u32)(F & 0x00000007) <<  8

int dmaKit_init(unsigned int RELE, unsigned int MFD, unsigned int STS, 
                unsigned int STD, unsigned int RCYC);

int dmaKit_chan_init(unsigned int channel);

#endif /* __DMAINIT_H__ */
