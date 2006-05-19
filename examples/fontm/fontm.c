//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// fontm.c - Example demonstrating ROM Font (FONTM) usage
//

#include "gsKit.h"
#include "dmaKit.h"
#include "malloc.h"

int main(void)
{
	u64 White, Black, BlackFont, WhiteFont, RedFont, GreenFont, BlueFont, BlueTrans, RedTrans, GreenTrans, WhiteTrans;
	GSGLOBAL *gsGlobal = gsKit_init_global(GS_MODE_NTSC_I);
//	GSGLOBAL *gsGlobal = gsKit_init_global(GS_MODE_PAL);
//	GSGLOBAL *gsGlobal = gsKit_init_global(GS_MODE_VGA_640_60);
	GSTEXTURE test;

	GSFONT *gsFont = gsKit_init_font(GSKIT_FTYPE_FONTM, NULL);

	dmaKit_init(D_CTRL_RELE_OFF,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);
	dmaKit_chan_init(DMA_CHANNEL_FROMSPR);
	dmaKit_chan_init(DMA_CHANNEL_TOSPR);
	
	Black = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x00,0x00);
	White = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x00,0x00);
	
	WhiteFont = GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00);
	BlackFont = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x80,0x00);
	RedFont = GS_SETREG_RGBAQ(0xFF,0x80,0x80,0x80,0x00);
	GreenFont = GS_SETREG_RGBAQ(0x80,0xFF,0x80,0x80,0x00);
	BlueFont = GS_SETREG_RGBAQ(0x80,0x80,0xFF,0x80,0x00);
        u64 TexCol = GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00);

	BlueTrans = GS_SETREG_RGBAQ(0x00,0x00,0xFF,0x40,0x00);
	RedTrans = GS_SETREG_RGBAQ(0xFF,0x00,0x00,0x60,0x00);
	GreenTrans = GS_SETREG_RGBAQ(0x00,0xFF,0x00,0x50,0x00);
	WhiteTrans = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x50,0x00);

        float x = 10;
        float y = 10;
        float width = 150;
        float height = 150;

	char tempstr[256];

	float VHeight = gsGlobal->Height;

        float x2 = (gsGlobal->Width - 10) - width;
        float y2 = VHeight - 10 - height;

        gsGlobal->PrimAlphaEnable = GS_SETTING_ON;

	gsKit_init_screen(gsGlobal);

	gsKit_font_upload(gsGlobal, gsFont);

	gsFont->FontM_Spacing = 0.95f;

	gsKit_texture_bmp(gsGlobal, &test, "host:test.bmp");
	test.Filter = GS_FILTER_LINEAR;

        gsKit_mode_switch(gsGlobal, GS_ONESHOT);

	while(1)
	{

                if( y <= 10  && (x + width) < (gsGlobal->Width - 10))
                        x+=5;
                else if( (y + height)  <  (VHeight - 10) && (x + width) >= (gsGlobal->Width - 10) )
                        y+=5;
                else if( (y + height) >=  (VHeight - 10) && x > 10 )
                        x-=5;
                else if( y > 10 && x <= 10 )
                        y-=5;

                if( y2 <= 10  && (x2 + width) < (gsGlobal->Width - 10))
                        x2+=5;
                else if( (y2 + height)  <  (VHeight - 10) && (x2 + width) >= (gsGlobal->Width - 10) )
                        y2+=5;
                else if( (y2 + height) >=  (VHeight - 10) && x2 > 10 )
                        x2-=5;
                else if( y2 > 10 && x2 <= 10 )
                        y2-=5;

		gsKit_clear(gsGlobal, White);

	        gsKit_prim_sprite_texture(      gsGlobal, &test,
                                                50.0f, 50.0f, 0.0f, 0.0f,
                                                gsGlobal->Width - 50.0f, gsGlobal->Height - 50.0f,
                                                test.Width, test.Height,
                                                1, TexCol);

		gsKit_prim_sprite(gsGlobal, x2, y2, x2 + width, y2 + height, 2, RedTrans);

		gsKit_font_print_scaled(gsGlobal, gsFont, 50, 50, 3, 0.85f, TexCol, 
			"1: ABCDEFGHIJKLM\n"
			"2: NOPQRSTUVWXYZ\n"
			"3: abcdefghijklm\n"
			"4: nopqrstuvwxyz\n"
			"5: 1234567890,./`\n"
			"6: ~!@#$%^&*()_<>\n"
			"7: +-=[]{}\\|;:\"'?\n"
			"8: \ele \ege \einf \emale \efemale \edegc \eyen \ecent \epound\n"
			"9: \eleft \eright \eup \edown \efleft \efright \efup \efdown\n"
			"10:\ehleft \ehright \ehup \ehdown \ems \eus \ens \edegf\n"
			"11:\embit \ehz \ekb \emb \egb \etb \f0855 \f0850\n"
			"Hello FONTM World!");

		sprintf(tempstr, "X =%d\t| Y =%d\nX2=%d\t| Y2=%d", (int)x, (int)y, (int)x2, (int)y2);

		gsKit_font_print_scaled(gsGlobal, gsFont, 50, 370, 3, 0.5f, TexCol, tempstr);

//		gsKit_font_print(gsGlobal, gsFont, 50, 50, 2.0f, TexCol, "\f0000");

		gsKit_prim_sprite(gsGlobal, x, y, x + width, y + height, 4, BlueTrans);

		gsKit_sync_flip(gsGlobal);

		gsKit_queue_exec(gsGlobal);
	}
	
	return 0;
}
