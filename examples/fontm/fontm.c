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
// Early units (SCPH-1xxxx and DESR-xxxx) lacks this feature

#include <stdio.h>
#include <malloc.h>

#include <gsKit.h>
#include <dmaKit.h>
#include <gsToolkit.h>

int main(int argc, char *argv[])
{
	u64 White, Black, BlackFont, WhiteFont, RedFont, GreenFont, BlueFont, BlueTrans, RedTrans, GreenTrans, WhiteTrans;
	GSGLOBAL *gsGlobal = gsKit_init_global();
	// GS_MODE_PAL_I
	// GS_MODE_VGA_640_60
	GSTEXTURE test;

	GSFONTM *gsFontM = gsKit_init_fontm();

	dmaKit_init(D_CTRL_RELE_OFF,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);

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

	gsKit_fontm_upload(gsGlobal, gsFontM);

	gsFontM->Spacing = 0.95f;

	test.Delayed = 1;
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

		gsKit_TexManager_bind(gsGlobal, &test);
		gsKit_prim_sprite_texture(gsGlobal, &test,
                                        50.0f, 50.0f, 0.0f, 0.0f,
                                        gsGlobal->Width - 50.0f, gsGlobal->Height - 50.0f,
                                        test.Width, test.Height,
                                        1, TexCol);

		gsKit_prim_sprite(gsGlobal, x2, y2, x2 + width, y2 + height, 2, RedTrans);
		gsKit_prim_sprite(gsGlobal, x2, y, x2 + width, y + height, 1, WhiteTrans);

		gsKit_fontm_print_scaled(gsGlobal, gsFontM, 50, 50, 3, 0.85f, WhiteFont,
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
			"11:\embit \ehz \ekb \emb \egb \etb \f0855 \f0850");

		sprintf(tempstr, "X =%d\t| Y =%d\nX2=%d\t| Y2=%d", (int)x, (int)y, (int)x2, (int)y2);

		gsKit_fontm_print_scaled(gsGlobal, gsFontM, 50, 340, 3, 0.6f, RedFont, tempstr);

		// gsKit_font_print(gsGlobal, gsFont, 50, 50, 2.0f, TexCol, "\f0000");

		gsKit_prim_sprite(gsGlobal, 418, 335, 422, 425, 3, Black);

		gsKit_fontm_print_scaled(gsGlobal, gsFontM, 420, 340, 4, 0.6f, BlueFont, "Left Aligned");
		gsFontM->Align = GSKIT_FALIGN_CENTER;
		gsKit_fontm_print_scaled(gsGlobal, gsFontM, 420, 370, 4, 0.6f, GreenFont, "Center Aligned");
		gsFontM->Align = GSKIT_FALIGN_RIGHT;
		gsKit_fontm_print_scaled(gsGlobal, gsFontM, 420, 400, 4, 0.60f, BlackFont, "Right Aligned");
		gsFontM->Align = GSKIT_FALIGN_LEFT;

		gsKit_prim_sprite(gsGlobal, x, y, x + width, y + height, 4, BlueTrans);
		gsKit_prim_sprite(gsGlobal, x, y2, x + width, y2 + height, 3, GreenTrans);

		gsKit_queue_exec(gsGlobal);
		gsKit_sync_flip(gsGlobal);
		gsKit_TexManager_nextFrame(gsGlobal);
	}

	return 0;
}
