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
// Portions taken from mrbrown's tutorial at:
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

#define VIF0_CHCR        (volatile u32 *)0x10008000
#define VIF0_MADR        (volatile u32 *)0x10008010
#define VIF0_SIZE        (volatile u32 *)0x10008020
#define VIF0_TADR        (volatile u32 *)0x10008030
#define VIF0_ASR0        (volatile u32 *)0x10008040
#define VIF0_ASR1        (volatile u32 *)0x10008050
#define VIF0_SADR        (volatile u32 *)0x10008080

#define VIF1_CHCR        (volatile u32 *)0x10009000
#define VIF1_MADR        (volatile u32 *)0x10009010
#define VIF1_SIZE        (volatile u32 *)0x10009020
#define VIF1_TADR        (volatile u32 *)0x10009030
#define VIF1_ASR0        (volatile u32 *)0x10009040
#define VIF1_ASR1        (volatile u32 *)0x10009050
#define VIF1_SADR        (volatile u32 *)0x10009080

#define GIF_CHCR	(volatile u32 *)0x1000A000
#define GIF_MADR	(volatile u32 *)0x1000A010
#define GIF_SIZE	(volatile u32 *)0x1000A020
#define GIF_TADR	(volatile u32 *)0x1000A030
#define GIF_ASR0	(volatile u32 *)0x1000A040
#define GIF_ASR1	(volatile u32 *)0x1000A050
#define GIF_SADR	(volatile u32 *)0x1000A080

#define FROMIPU_CHCR	(volatile u32 *)0x1000B000
#define FROMIPU_MADR	(volatile u32 *)0x1000B010
#define FROMIPU_QWC	(volatile u32 *)0x1000B020

#define TOIPU_CHCR    (volatile u32 *)0x1000B400
#define TOIPU_MADR    (volatile u32 *)0x1000B410
#define TOIPU_QWC     (volatile u32 *)0x1000B420
#define TOIPU_TADR    (volatile u32 *)0x1000B430

#define SIF0_CHCR    (volatile u32 *)0x1000C000
#define SIF0_MADR    (volatile u32 *)0x1000C010
#define SIF0_QWC     (volatile u32 *)0x1000C020

#define SIF1_CHCR    (volatile u32 *)0x1000C400
#define SIF1_MADR    (volatile u32 *)0x1000C410
#define SIF1_QWC     (volatile u32 *)0x1000C420
#define SIF1_TADR    (volatile u32 *)0x1000C430

#define SIF2_CHCR    (volatile u32 *)0x1000C800
#define SIF2_MADR    (volatile u32 *)0x1000C810
#define SIF2_QWC     (volatile u32 *)0x1000C820

#define FROMSPR_CHCR     (volatile u32 *)0x1000D000
#define FROMSPR_MADR     (volatile u32 *)0x1000D010
#define FROMSPR_QWC      (volatile u32 *)0x1000D020
#define FROMSPR_SADR     (volatile u32 *)0x1000D080

#define TOSPR_CHCR     (volatile u32 *)0x1000D400
#define TOSPR_MADR     (volatile u32 *)0x1000D410
#define TOSPR_QWC      (volatile u32 *)0x1000D420
#define TOSPR_TADR     (volatile u32 *)0x1000D430
#define TOSPR_SADR     (volatile u32 *)0x1000D480

int dmaKit_init(unsigned int RELE, unsigned int MFD, unsigned int STS, 
                unsigned int STD, unsigned int RCYC);
int dmaKit_chan_init(unsigned int channel);

#endif /* __DMAINIT_H__ */
