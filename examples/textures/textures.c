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
	u64 White, Black, Red, Green, Blue, BlueTrans, RedTrans, GreenTrans, WhiteTrans;

	GSGLOBAL gsGlobal;
	GSTEXTURE Tex1, Tex2;

	dmaKit_init(D_CTRL_RELE_ON,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);

	White = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x00,0x00);
	Black = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x00,0x00);
	Red = GS_SETREG_RGBAQ(0xFF,0x00,0x00,0x00,0x00);
	Green = GS_SETREG_RGBAQ(0x00,0xFF,0x00,0x00,0x00);
	Blue = GS_SETREG_RGBAQ(0x00,0x00,0xFF,0x00,0x00);

	BlueTrans = GS_SETREG_RGBAQ(0x00,0x00,0xFF,0x40,0x00);
	RedTrans = GS_SETREG_RGBAQ(0xFF,0x00,0x00,0x60,0x00);
	GreenTrans = GS_SETREG_RGBAQ(0x00,0xFF,0x00,0x50,0x00);
	WhiteTrans = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x50,0x00);

	/* Generic Values */
	gsGlobal.Mode = GS_MODE_NTSC;
	gsGlobal.Interlace = GS_NONINTERLACED;
	gsGlobal.Field = GS_FRAME;
	gsGlobal.Aspect = GS_ASPECT_4_3;
	gsGlobal.Width = 640;
	gsGlobal.Height = 448;
	gsGlobal.OffsetX = 2048;
	gsGlobal.OffsetY = 2048;
	gsGlobal.StartX = 0;
	gsGlobal.StartY = -5;
	gsGlobal.PSM = GS_PSM_CT16;
	gsGlobal.PSMZ = GS_PSMZ_16;
	gsGlobal.ActiveBuffer = 1;
	gsGlobal.PrimFogEnable = 0;
	gsGlobal.PrimAAEnable = 0;
	gsGlobal.PrimAlphaEnable = 1;
	gsGlobal.PrimAlpha = 1;
	gsGlobal.PrimContext = 0;

	/* BGColor Register Values */
	gsGlobal.BGColor.Red = 0x00;
	gsGlobal.BGColor.Green = 0x00;
	gsGlobal.BGColor.Blue = 0x00;

	/* TEST Register Values */
	gsGlobal.Test.ATE = 0;
	gsGlobal.Test.ATST = 1;
	gsGlobal.Test.AREF = 0x80;
	gsGlobal.Test.AFAIL = 0;
	gsGlobal.Test.DATE = 0;
	gsGlobal.Test.DATM = 0;
	gsGlobal.Test.ZTE = 1;
	gsGlobal.Test.ZTST = 2;

	gsGlobal.Clamp.WMS = GS_CMODE_CLAMP;
	gsGlobal.Clamp.WMT = GS_CMODE_CLAMP;
/*
	// These are only relevant if you are using REGION_CLAMP or REGION_REPEAT
	gsGlobal.Clamp.MINU =
	gsGlobal.Clamp.MAXU =
	gsGlobal.Clamp.MINV =
	gsGlobal.Clamp.MAXV =
*/
	gsKit_init_screen(&gsGlobal);
	gsKit_clear(&gsGlobal, White);
	
	Tex1.Width = 256;
	Tex1.Height = 256;
	Tex1.PSM = GS_PSM_CT24;

	gsKit_texture_raw(&gsGlobal, &Tex1, "host:bitmap.raw");
	gsKit_texture_bmp(&gsGlobal, &Tex2, "host:bsdgirl.bmp");

	while(1){
		gsKit_clear(&gsGlobal, White);

                gsKit_prim_sprite_texture(&gsGlobal, &Tex1, 20,  // X1
                                                            50,  // Y2    
                                                            0,  // U1
                                                            0,  // V1
                                                            276, // X2
                                                            306, // Y2
                                                            256, // U2
                                                            256, // V2
                                                            1,
                                                            0x80808080);

                gsKit_prim_sprite_texture(&gsGlobal, &Tex2, 310,  // X1
                                                            0,  // Y2
                                                            0,  // U1
                                                            0,  // V1
                                                            630, // X2
                                                            480, // Y2
                                                            320, // U2
                                                            480, // V2
                                                            1,
                                                            0x80808080);

		gsKit_sync_flip(&gsGlobal);
	}
	
	return 0;
}
