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
	GSGLOBAL *gsGlobal = gsKit_init_global(GS_MODE_VGA_640_85);

	float x = 10;
	float y = 10;
	float width = 150;
	float height = 150;

	float *LineStrip;
	float *LineStripPtr;
	float *TriStrip;
	float *TriStripPtr;
	float *TriFanPtr;
	float *TriFan;

	LineStripPtr = LineStrip = malloc(12 * sizeof(float));
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

	TriStripPtr = TriStrip = malloc(12 * sizeof(float));
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
	
	TriFanPtr = TriFan = malloc(16 * sizeof(float));
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

	gsKit_init_screen(gsGlobal);

	while(1){
		gsKit_clear(gsGlobal, White);

		gsKit_set_test(gsGlobal, GS_ZTEST_OFF);
	
		gsKit_prim_line_strip(gsGlobal, LineStripPtr, 6, 1.0, Black);

		gsKit_prim_triangle_strip(gsGlobal, TriStripPtr, 6, 1.0, Red);

		gsKit_prim_line(gsGlobal, 525.0, 125.0, 575.0, 125.0, 1.0, Black);
		gsKit_prim_line(gsGlobal, 550.0, 150.0, 600.0, 100.0, 1.0, Black);

		gsKit_prim_point(gsGlobal, 575.0, 75.0, 1.0, Black);
		gsKit_prim_point(gsGlobal, 600.0, 100.0, 1.0, Black);
		gsKit_prim_point(gsGlobal, 625.0, 125.0, 1.0, Black);

		gsKit_prim_quad(gsGlobal, 150.0, 150.0, 
					   150.0, 400.0,
					   450.0, 150.0,
					   450.0, 400.0, 2.0, Green);

		gsKit_set_test(gsGlobal, GS_ZTEST_ON);

		gsKit_prim_triangle_fan(gsGlobal, TriFanPtr, 8, 5.0, Black);

		gsKit_prim_quad_gouraud(gsGlobal, 500.0, 250.0, 
						   500.0, 350.0, 
						   600.0, 250.0,
						   600.0, 350.0, 2.0,
						   Red, Green, Blue, Black);

		gsKit_prim_triangle_gouraud(gsGlobal, 280.0, 200.0,
						       280.0, 350.0,
						       180.0, 350.0, 5.0, 
						       Blue, Red, White);

		gsKit_prim_triangle(gsGlobal, 300.0, 200.0, 300.0, 350.0, 400.0, 350.0, 3.0, Red);

		gsKit_prim_sprite(gsGlobal, 400.0, 100.0, 500.0, 200.0, 5.0, Red);

		if( y <= 10  && (x + width) < (gsGlobal->Width - 10))
			x+=10;
		else if( (y + height)  <  (gsGlobal->Height - 10) && (x + width) >= (gsGlobal->Width - 10) )
			y+=10;		
		else if( (y + height) >=  (gsGlobal->Height - 10) && x > 10 )
			x-=10;
		else if( y > 10 && x <= 10 )
			y-=10;

		gsKit_prim_sprite(gsGlobal, x, y, x + width, y + height, 4.0, BlueTrans);

		gsKit_prim_sprite(gsGlobal, 100.0, 100.0, 200.0, 200.0, 5.0, RedTrans);

		gsKit_sync_flip(gsGlobal);

	}
	
	return 0;
}
