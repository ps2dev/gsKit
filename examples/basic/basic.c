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

	BlueTrans = GS_SETREG_RGBAQ(0x00,0x00,0xFF,0x50,0x00);

	printf("blah got here\n");

	/* Generic Values */
	gsGlobal.Width = 640;
	gsGlobal.Height = 448;
	gsGlobal.Aspect = GS_ASPECT_4_3;
	gsGlobal.OffsetX = 2048;
	gsGlobal.OffsetY = 2048;
	gsGlobal.PSM = 0;
	gsGlobal.ActiveBuffer = 1;
	gsGlobal.PrimAlphaEnable = 1;
	gsGlobal.PrimAlpha = 1;
	gsGlobal.PrimContext = 0;

	/* BGColor Register Values */
	gsGlobal.BGColor.Red = 0x00;
	gsGlobal.BGColor.Green = 0x00;
	gsGlobal.BGColor.Blue = 0x00;

	/* TEST Register Values */
	gsGlobal.Test.ATE = 1;
	gsGlobal.Test.ATST = 1;
	gsGlobal.Test.AREF = 0x80;
	gsGlobal.Test.AFAIL = 0;
	gsGlobal.Test.DATE = 0;
	gsGlobal.Test.DATM = 0;
	gsGlobal.Test.ZTE = 1;
	gsGlobal.Test.ZTST = 2;

	printf("blah got here2 \n");
	gsKit_init_screen(&gsGlobal, GS_INTERLACED, GS_MODE_NTSC, GS_FIELD);
	printf("blah got here3 \n");

	int x = 10;
	int y = 10;
	int width = 150;
	int height = 150;

	while(1){
		gsKit_clear(&gsGlobal, White);
		printf("DEBUG: SCREEN CLEARED\n");

		gsKit_set_test(&gsGlobal, GS_ZTEST_OFF);
	
		gsKit_prim_sprite(&gsGlobal, 100, 100, 200, 200, 1, Blue);
		printf("DEBUG: PRIM 1 DRAWN\n");
		gsKit_prim_sprite(&gsGlobal, 150, 150, 450, 400, 3, Green);
		printf("DEBUG: PRIM 2 DRAWN\n");

		gsKit_set_test(&gsGlobal, GS_ZTEST_ON);

		gsKit_prim_sprite(&gsGlobal, 400, 100, 500, 200, 5, Red);
		printf("DEBUG: PRIM 3 DRAWN\n");

		if( y <= 10  && (x + width) < (gsGlobal.Width - 10))
			x+=10;
		else if( (y + height)  <  (gsGlobal.Height - 10) && (x + width) >= (gsGlobal.Width - 10) )
			y+=10;		
		else if( (y + height) >=  (gsGlobal.Height - 10) && x > 10 )
			x-=10;
		else if( y > 10 && x <= 10 )
			y-=10;

		gsKit_prim_sprite(&gsGlobal, x, y, x + width, y + height, 4, BlueTrans);
		printf("DEBUG: PRIM 4 DRAWN\n");

		gsKit_sync_flip(&gsGlobal);

		printf("DEBUG: SYNC FLIP 2\n");
		printf("FINISH\n");
	}
	
	return 0;
}
