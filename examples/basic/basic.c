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
#include "malloc.h"

int main(void)
{
	u64 White, Black, Red, Green, Blue, BlueTrans, RedTrans, GreenTrans, WhiteTrans;

	int x = 10;
	int y = 10;
	int width = 150;
	int height = 150;

	int *LineStrip;
	int *LineStripPtr;
	int *TriStrip;
	int *TriStripPtr;
	int *TriFanPtr;
	int *TriFan;

	LineStripPtr = LineStrip = malloc(12 * sizeof(int));
	*LineStrip++ = 75;	// Segment 1 X
	*LineStrip++ = 250;	// Segment 1 Y
	*LineStrip++ = 125;	// Segment 2 X
	*LineStrip++ = 290;	// Segment 2 Y
	*LineStrip++ = 100;	// Segment 3 X
	*LineStrip++ = 350;	// Segment 3 Y
	*LineStrip++ = 50;	// Segment 4 X
	*LineStrip++ = 350;	// Segment 4 Y
	*LineStrip++ = 25;	// Segment 6 X
	*LineStrip++ = 290;	// Segment 6 X
	*LineStrip++ = 75;	// Segment 6 Y
	*LineStrip++ = 250;	// Segment 6 Y

        TriStripPtr = TriStrip = malloc(12 * sizeof(int));
        *TriStrip++ = 550;
        *TriStrip++ = 100;
        *TriStrip++ = 525;
        *TriStrip++ = 125;
        *TriStrip++ = 575;
        *TriStrip++ = 125;
        *TriStrip++ = 550;
        *TriStrip++ = 150;
        *TriStrip++ = 600;
	*TriStrip++ = 150;
        *TriStrip++ = 575;
        *TriStrip++ = 175;

        TriFanPtr = TriFan = malloc(16 * sizeof(int));
	*TriFan++ = 300;
	*TriFan++ = 100;
	*TriFan++ = 225;
	*TriFan++ = 100;
	*TriFan++ = 235;
	*TriFan++ = 75;
	*TriFan++ = 265;
	*TriFan++ = 40;
	*TriFan++ = 300;
	*TriFan++ = 25;
	*TriFan++ = 335;
	*TriFan++ = 40;
	*TriFan++ = 365;
	*TriFan++ = 75;
	*TriFan++ = 375;
	*TriFan++ = 100;

	GSGLOBAL gsGlobal;

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
	gsGlobal.StartX = -5;
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

	gsKit_init_screen(&gsGlobal);

	while(1){
		gsKit_clear(&gsGlobal, White);

		gsKit_set_test(&gsGlobal, GS_ZTEST_OFF);
	
		gsKit_prim_line_strip(&gsGlobal, LineStripPtr, 6, 1, Black);

		gsKit_prim_triangle_strip(&gsGlobal, TriStripPtr, 6, 1, Red);

		gsKit_prim_line(&gsGlobal, 525, 125, 575, 125, 1, Black);
		gsKit_prim_line(&gsGlobal, 550, 150, 600, 100, 1, Black);

		gsKit_prim_point(&gsGlobal, 575, 75, 1, Black);
		gsKit_prim_point(&gsGlobal, 600, 100, 1, Black);
		gsKit_prim_point(&gsGlobal, 625, 125, 1, Black);

		gsKit_prim_quad(&gsGlobal, 150, 150, 
					   150, 400,
					   450, 150,
					   450, 400, 2, Green);

		gsKit_set_test(&gsGlobal, GS_ZTEST_ON);

		gsKit_prim_triangle_fan(&gsGlobal, TriFanPtr, 8, 5, Black);

		gsKit_prim_quad_gouraud(&gsGlobal, 500, 250, 
						   500, 350, 
						   600, 250,
						   600, 350, 2,
						   Red, Green, Blue, Black);

		gsKit_prim_triangle_gouraud(&gsGlobal, 280, 200, 
						       280, 350, 
						       180, 350, 5, 
						       Blue, Red, White);

		gsKit_prim_triangle(&gsGlobal, 300, 200, 300, 350, 400, 350, 3, Red);

		gsKit_prim_sprite(&gsGlobal, 400, 100, 500, 200, 5, Red);

		if( y <= 10  && (x + width) < (gsGlobal.Width - 10))
			x+=10;
		else if( (y + height)  <  (gsGlobal.Height - 10) && (x + width) >= (gsGlobal.Width - 10) )
			y+=10;		
		else if( (y + height) >=  (gsGlobal.Height - 10) && x > 10 )
			x-=10;
		else if( y > 10 && x <= 10 )
			y-=10;

		gsKit_prim_sprite(&gsGlobal, x, y, x + width, y + height, 4, BlueTrans);

		gsKit_prim_sprite(&gsGlobal, 100, 100, 200, 200, 5, RedTrans);

		gsKit_sync_flip(&gsGlobal);

	}
	
	return 0;
}
