//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// dmaCore.c - Core DMA Routines.
// Parts taken from emoon's BreakPoint Demo Library
//

#include "dmaKit.h"
#include "dmaArrays.h"

#include <stdio.h>
#include <kernel.h>

int dmaKit_wait(u16 channel, u32 timeout)
{
#ifdef GSKIT_DEBUG
    printf("Waiting for DMA Channel %i - %s\n", channel, DMA_NAME[channel]);
#endif

    while ((*(volatile u32 *)DMA_CHCR[channel]) & 0x00000100) {
        if (timeout != 0) {
            if (timeout == 1) {
                printf("Timed out waiting for DMA Channel %i - %s to be clear.\n", channel, DMA_NAME[channel]);
                return -1;
            }
            timeout--;
        }
    }

#ifdef GSKIT_DEBUG
    printf("DMA Channel %i - %s is now clear.\n", channel, DMA_NAME[channel]);
#endif

    return 0;
}

void dmaKit_wait_fast(void)
{
    asm __volatile__("sync.l; sync.p;"
                     "0:"
                     "bc0t 0f; nop;"
                     "bc0t 0f; nop;"
                     "bc0t 0f; nop;"
                     "bc0f 0b; nop;"
                     "0:");
}

void dmaKit_send(u16 channel, void *data, u32 size)
{
#ifdef GSKIT_DEBUG
    printf("Sending to DMA Channel %i - %s\n", channel, DMA_NAME[channel]);
#endif

    DMA_SET_CIS(1 << channel);

    SyncDCache(data, data + size * 16);

    if (DMA_QWC[channel])
        *(volatile u32 *)DMA_QWC[channel] = size;
    *(volatile u32 *)DMA_MADR[channel] = DMA_SET_MADR((u32)data, 0);
    *(volatile u32 *)DMA_CHCR[channel] = DMA_SET_CHCR(1,                        // Direction
                                                      0,                        // ChainMode
                                                      0,                        // Address Stack Pointer
                                                      DMA_TAG_ENABLE[channel],  // Transfer DMA Tag
                                                      0,                        // Enable Interrupts
                                                      1,                        // Start DMA
                                                      0);                       // Priority Control Enable??

#ifdef GSKIT_DEBUG
    printf("Sent to DMA Channel\n");
#endif

    return;
}

void dmaKit_send_ucab(u16 channel, void *data, u32 size)
{
#ifdef GSKIT_DEBUG
    printf("Sending to DMA Channel %i - %s\n", channel, DMA_NAME[channel]);
#endif

    DMA_SET_CIS(1 << channel);

    if (DMA_QWC[channel])
        *(volatile u32 *)DMA_QWC[channel] = size;
    *(volatile u32 *)DMA_MADR[channel] = DMA_SET_MADR((u32)data - 0x30000000, 0);
    *(volatile u32 *)DMA_CHCR[channel] = DMA_SET_CHCR(1,                        // Direction
                                                      0,                        // ChainMode
                                                      0,                        // Address Stack Pointer
                                                      DMA_TAG_ENABLE[channel],  // Transfer DMA Tag
                                                      0,                        // Enable Interrupts
                                                      1,                        // Start DMA
                                                      0);                       // Priority Control Enable??

#ifdef GSKIT_DEBUG
    printf("Sent to DMA Channel\n");
#endif

    return;
}

void dmaKit_send_spr(u16 channel, void *data, u32 size)
{
#ifdef GSKIT_DEBUG
    printf("Sending to DMA Channel w/ SPR %i - %s\n", channel, DMA_NAME[channel]);
#endif

    DMA_SET_CIS(1 << channel);

    if (DMA_QWC[channel])
        *(volatile u32 *)DMA_QWC[channel] = size;
    *(volatile u32 *)DMA_MADR[channel] = DMA_SET_MADR((u32)data, 1);
    *(volatile u32 *)DMA_CHCR[channel] = DMA_SET_CHCR(1,                        // Direction
                                                      0,                        // ChainMode
                                                      0,                        // Address Stack Pointer
                                                      DMA_TAG_ENABLE[channel],  // Transfer DMA Tag
                                                      0,                        // Enable Interrupts
                                                      1,                        // Start DMA
                                                      0);                       // Priority Control Enable??
#ifdef GSKIT_DEBUG
    printf("Sent to DMA Channel w/ SPR\n");
#endif

    return;
}

void dmaKit_send_chain(u16 channel, void *data, u32 size)
{
#ifdef GSKIT_DEBUG
    printf("Sending to DMA Channel in Chain Mode %i - %s\n", channel, DMA_NAME[channel]);
#endif

    DMA_SET_CIS(1 << channel);

    SyncDCache(data, data + size * 16);

    if (DMA_QWC[channel])
        *(volatile u32 *)DMA_QWC[channel] = 0;

    *(volatile u32 *)DMA_TADR[channel] = DMA_SET_TADR((u32)data, 0);
    *(volatile u32 *)DMA_CHCR[channel] = DMA_SET_CHCR(1,                        // Direction
                                                      1,                        // ChainMode
                                                      0,                        // Address Stack Pointer
                                                      DMA_TAG_ENABLE[channel],  // Transfer DMA Tag
                                                      0,                        // Enable Interrupts
                                                      1,                        // Start DMA
                                                      0);                       // Priority Control Enable??

#ifdef GSKIT_DEBUG
    printf("Sent to DMA Channel in Chain Mode\n");
#endif
    return;
}

void dmaKit_send_chain_ucab(u16 channel, void *data)
{
#ifdef GSKIT_DEBUG
    printf("Sending to DMA Channel in Chain Mode from UCAB %i - %s\n", channel, DMA_NAME[channel]);
#endif

    DMA_SET_CIS(1 << channel);

    if (DMA_QWC[channel])
        *(volatile u32 *)DMA_QWC[channel] = 0;

    *(volatile u32 *)DMA_TADR[channel] = DMA_SET_TADR((u32)data - 0x30000000, 0);
    *(volatile u32 *)DMA_CHCR[channel] = DMA_SET_CHCR(1,                        // Direction
                                                      1,                        // ChainMode
                                                      0,                        // Address Stack Pointer
                                                      DMA_TAG_ENABLE[channel],  // Transfer DMA Tag
                                                      0,                        // Enable Interrupts
                                                      1,                        // Start DMA
                                                      0);                       // Priority Control Enable??

#ifdef GSKIT_DEBUG
    printf("Sent to DMA Channel in Chain Mode from UCAB\n");
#endif
    return;
}

void dmaKit_send_chain_spr(u16 channel, void *data)
{
#ifdef GSKIT_DEBUG
    printf("Sending to DMA Channel in Chain Mode w/Scratchpad %i - %s\n", channel, DMA_NAME[channel]);
#endif

    DMA_SET_CIS(1 << channel);

    if (DMA_QWC[channel])
        *(volatile u32 *)DMA_QWC[channel] = 0;
    *(volatile u32 *)DMA_TADR[channel] = DMA_SET_TADR((u32)data, 1);
    *(volatile u32 *)DMA_CHCR[channel] = DMA_SET_CHCR(1,                        // Direction
                                                      1,                        // ChainMode
                                                      0,                        // Address Stack Pointer
                                                      DMA_TAG_ENABLE[channel],  // Transfer DMA Tag
                                                      0,                        // Enable Interrupts
                                                      1,                        // Start DMA
                                                      0);                       // Priority Control Enable??

#ifdef GSKIT_DEBUG
    printf("Sent to DMA Channel in Chain Mode w/Scratchpad\n");
#endif
    return;
}

void dmaKit_get_spr(u16 channel, void *data, void *dest, u32 size)
{
#ifdef GSKIT_DEBUG
    printf("Sending to DMA Channel %i - %s\n", channel, DMA_NAME[channel]);
#endif

    DMA_SET_CIS(1 << channel);

    if (DMA_QWC[channel])
        *(volatile u32 *)DMA_QWC[channel] = size;

    *(volatile u32 *)DMA_SADR[channel] = DMA_SET_SADR((u32)data & 0x3fff);
    *(volatile u32 *)DMA_MADR[channel] = DMA_SET_MADR((u32)dest & 0x0fffffff, 0);
    *(volatile u32 *)DMA_CHCR[channel] = DMA_SET_CHCR(0,                        // Direction
                                                      0,                        // ChainMode
                                                      0,                        // Address Stack Pointer
                                                      DMA_TAG_ENABLE[channel],  // Transfer DMA Tag
                                                      0,                        // Enable Interrupts
                                                      1,                        // Start DMA
                                                      0);                       // Priority Control Enable??

#ifdef GSKIT_DEBUG
    printf("Sent to DMA Channel\n");
#endif

    return;
}
