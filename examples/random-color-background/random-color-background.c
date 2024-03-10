//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2024 - Lucas Teles "PS2DEV" <ps2dev3@gmail.com>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// YouTube - https://www.youtube.com/@ps2dev
// Working - PCSX2 v1.7.5530
//
// random-color-background.c - Example demonstrating random color on background
//

#include <stdio.h>
#include <unistd.h>
#include <gsKit.h>
#include <gsToolkit.h>

int main() {
  GSGLOBAL *gsGlobal;

  dmaKit_init(
    D_CTRL_RELE_OFF,
    D_CTRL_MFD_OFF,
    D_CTRL_STS_UNSPEC,
		D_CTRL_STD_OFF,
    D_CTRL_RCYC_8,
    1 << DMA_CHANNEL_GIF
  );

  u64 colors[3] = {
    GS_SETREG_RGBAQ(50, 168, 90, 0, 0),
    GS_SETREG_RGBAQ(222, 20, 30, 0, 0),
    GS_SETREG_RGBAQ(240, 218, 26, 0, 0)
  };

  gsGlobal = gsKit_init_global();
  gsKit_init_screen(gsGlobal);

  int counter = 0;

  while (true) {
    gsKit_clear(gsGlobal, colors[counter]);

    counter = (counter + 1) % 3;

    gsKit_queue_exec(gsGlobal);
    gsKit_sync_flip(gsGlobal);

    sleep(1);    
  }

  return 0;
}