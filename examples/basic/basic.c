//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// basic.c - Example demonstrating basic gsKit operation.
//

#include "gsKit.h"
#include "dmaKit.h"

int main(void)
{
	u64 White, Red, Green, Blue, BlueTrans;

	GSGLOBAL gsGlobal;

	dmaKit_init(D_CTRL_RELE_ON,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);

	White = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x00,0x00);
	Red = GS_SETREG_RGBAQ(0xFF,0x00,0x00,0x00,0x00);
	Green = GS_SETREG_RGBAQ(0x00,0xFF,0x00,0x00,0x00);
	Blue = GS_SETREG_RGBAQ(0x00,0x00,0xFF,0x00,0x00);

	BlueTrans = GS_SETREG_RGBAQ(0x00,0x00,0xFF,0xEE,0x00);

	/* Generic Values */
	gsGlobal.Width = 640;
	gsGlobal.Height = 448;
	gsGlobal.Aspect = GS_ASPECT_4_3;
	gsGlobal.OffsetX = 2048;
	gsGlobal.OffsetY = 2048;
	gsGlobal.PSM = 0;
	gsGlobal.ActiveBuffer = 0;
	gsGlobal.PrimAlphaEnable = 1;
	gsGlobal.PrimContext = 1;

	/* BGColor Register Values */
	gsGlobal.BGColor.Red = 0x00;
	gsGlobal.BGColor.Green = 0x00;
	gsGlobal.BGColor.Blue = 0x00;

	/* TEST Register Values */
	gsGlobal.Test.ATE = 1;
	gsGlobal.Test.ATST = 7;
	gsGlobal.Test.AREF = 0xff;
	gsGlobal.Test.AFAIL = 0;
	gsGlobal.Test.DATE = 0;
	gsGlobal.Test.DATM = 0;
	gsGlobal.Test.ZTE = 1;
	gsGlobal.Test.ZTST = 2;

	gsGlobal = gsKit_init_screen(gsGlobal, GS_INTERLACED, GS_MODE_NTSC, GS_FIELD);

	gsKit_clear(gsGlobal, White);
	printf("DEBUG: SCREEN CLEARED\n");

	gsGlobal = gsKit_sync_flip(gsGlobal);

	gsKit_set_test(gsGlobal, GS_ZTEST_OFF);
	
	gsKit_prim_sprite(gsGlobal, 100, 100, 200, 200, 1, Blue);
	printf("DEBUG: PRIM 1 DRAWN\n");
	gsKit_prim_sprite(gsGlobal, 200, 200, 400, 400, 1, Green);
	printf("DEBUG: PRIM 2 DRAWN\n");

//	gsKit_set_test(gsGlobal, GS_ZTEST_ON);

	gsKit_prim_sprite(gsGlobal, 400, 100, 500, 200, 3, Red);
	printf("DEBUG: PRIM 3 DRAWN\n");
	gsKit_prim_sprite(gsGlobal, 150, 150, 450, 300, 2, BlueTrans);
	printf("DEBUG: PRIM 4 DRAWN\n");

	gsGlobal = gsKit_sync_flip(gsGlobal);
	printf("DEBUG: SYNC FLIP 2\n");
	printf("FINISH\n");

	while(1);

	return 0;
}
