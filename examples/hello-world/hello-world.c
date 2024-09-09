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
// hello-world.c - Example demonstrating Hello World
//

#include <stdio.h>
#include <gsKit.h>
#include <gsToolkit.h>

int main() {
  u64 whiteFont;
  
  GSGLOBAL *gsGlobal;
  GSFONTM *gsFontM;

  dmaKit_init(
    D_CTRL_RELE_OFF,
    D_CTRL_MFD_OFF,
    D_CTRL_STS_UNSPEC,
		D_CTRL_STD_OFF,
    D_CTRL_RCYC_8,
    1 << DMA_CHANNEL_GIF
  );

	dmaKit_chan_init(DMA_CHANNEL_GIF);

  whiteFont = GS_SETREG_RGBAQ(255, 255, 255, 0, 0);

  gsGlobal = gsKit_init_global();
  gsFontM = gsKit_init_fontm();

  gsKit_init_screen(gsGlobal);
  gsKit_fontm_upload(gsGlobal, gsFontM);

  while (true) {
    gsKit_fontm_print_scaled(
      gsGlobal,
      gsFontM,
      20,
      20,
      0,
      0.85f,
      whiteFont,
      "Hello World!"
    );

    gsKit_queue_exec(gsGlobal);
    gsKit_sync_flip(gsGlobal);
  }

  return 0;
}