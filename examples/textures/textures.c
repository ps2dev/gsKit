//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// textures.c - Example demonstrating gsKit texture operation.
//

#include "gsKit.h"
#include "dmaKit.h"
#include "malloc.h"

int main(void)
{
	GSGLOBAL *gsGlobal = gsKit_init_global();
	GSTEXTURE Tex1, Tex2;
	u64 White = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x00,0x00);

	dmaKit_init(D_CTRL_RELE_ON,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);

	gsKit_set_clamp(gsGlobal, GS_CMODE_CLAMP);
	
	gsKit_init_screen(gsGlobal);
	gsKit_clear(gsGlobal, White);
	
	Tex1.Width = 256.0;
	Tex1.Height = 256.0;
	Tex1.PSM = GS_PSM_CT24;

	gsKit_texture_raw(gsGlobal, &Tex1, "host:bitmap.raw");
	gsKit_texture_bmp(gsGlobal, &Tex2, "host:bsdgirl.bmp");

	while(1){
		gsKit_clear(gsGlobal, White);

                gsKit_prim_sprite_texture(gsGlobal, &Tex1, 20.0,  // X1
                                                            50.0,  // Y2    
                                                            0.0,  // U1
                                                            0.0,  // V1
                                                            Tex1.Width + 20.0, // X2
                                                            Tex1.Height + 50.0, // Y2
                                                            Tex1.Width, // U2
                                                            Tex1.Height, // V2
                                                            1.0,
                                                            0x80808080);

                gsKit_prim_sprite_texture(gsGlobal, &Tex2, 310.0,  // X1
                                                            50.0,  // Y2
                                                            0.0,  // U1
                                                            0.0,  // V1
                                                            Tex2.Width + 310.0, // X2
                                                            Tex2.Height +  50.0, // Y2
                                                            Tex2.Width, // U2
                                                            Tex2.Height, // V2
                                                            1.0,
                                                            0x80808080);

		gsKit_sync_flip(gsGlobal);
	}
	
	return 0;
}
