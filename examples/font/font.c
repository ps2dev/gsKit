//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// font.c - Example demonstrating basic font operation.
//

#include "gsKit.h"
#include "dmaKit.h"
#include "malloc.h"

int main(void)
{
	u64 White, Black, Red, Green, Blue, BlueTrans, RedTrans, GreenTrans, WhiteTrans, Texture;
	GSGLOBAL gsGlobal;
	GSFONT gsFont, gsFont2;
	
	dmaKit_init(D_CTRL_RELE_ON,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);

	Texture = GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00);

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
	gsGlobal.BGColor.Blue = 0x0;

	/* TEST Register Values */
	gsGlobal.Test.ATE = 0;
	gsGlobal.Test.ATST = 1;
	gsGlobal.Test.AREF = 0x80;
	gsGlobal.Test.AFAIL = 0;
	gsGlobal.Test.DATE = 0;
	gsGlobal.Test.DATM = 0;
	gsGlobal.Test.ZTE = 1;
	gsGlobal.Test.ZTST = 2;

	gsFont.Path = "host:lucida.fnt";
	gsFont.Type = GSKIT_FTYPE_FNT;

	gsFont2.Path = "host:arial.fnt";
	gsFont2.Type = GSKIT_FTYPE_FNT;

	gsKit_init_screen(&gsGlobal);
	gsKit_clear(&gsGlobal, White);

        gsKit_font_upload(&gsGlobal, &gsFont);
        gsKit_font_upload(&gsGlobal, &gsFont2);

	while(1){
		gsKit_clear(&gsGlobal, White);

//		gsKit_font_print(&gsGlobal, &gsFont, 25, 150, 2, White, "Hello World!");

		gsKit_prim_sprite_texture(&gsGlobal, &gsFont.Texture, 50, 50, 0, 0, 
								      gsFont.Texture.Width + 50, gsFont.Texture.Height + 50, 
								      gsFont.Texture.Width, gsFont.Texture.Height, 1, 0x80808080);

		gsKit_prim_sprite_texture(&gsGlobal, &gsFont2.Texture, 320, 50, 0, 0, 
				          gsFont.Texture.Width + 320, gsFont.Texture.Height + 50, 
					  gsFont.Texture.Width, gsFont.Texture.Height, 1, 0x80808080);

		gsKit_sync_flip(&gsGlobal);

	}
	
	return 0;
}
