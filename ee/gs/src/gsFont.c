//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsFont.c - Font loading, manipulation, handling, and printing.
//

#include <stdio.h>
#include <string.h>

#include "gsKit.h"

int gsKit_font_upload_raw(GSGLOBAL *gsGlobal, GSFONT *gsFont)
{
	int i;

	if( gsFont->Type == GSKIT_FTYPE_FNT )
	{
		if( gsKit_texture_fnt_raw(gsGlobal, gsFont) == -1 )
		{
			printf("Error uploading font!\n");
			return -1; 
		}
		gsFont->Additional=malloc( 0x100 );
		for (i=0; i<0x100; i++) {
			gsFont->Additional[i] = gsFont->CharWidth;
		}

		return 0;		
	}

	return -1; //type unknown
}

int gsKit_font_upload(GSGLOBAL *gsGlobal, GSFONT *gsFont)
{
	int i;

	if( gsFont->RawData ) {
		return gsKit_font_upload_raw(gsGlobal, gsFont);
	}

	if( gsFont->Type == GSKIT_FTYPE_FNT )
	{
		if( gsKit_texture_fnt(gsGlobal, gsFont) == -1 )
		{
			printf("Error uploading font!\n");
			return -1; 
		}
		gsFont->Additional=malloc( 0x100 );
		for (i=0; i<0x100; i++) {
			gsFont->Additional[i] = gsFont->CharWidth;
		}

		return 0;
	}
	else if( gsFont->Type == GSKIT_FTYPE_BMP_DAT )
	{
		if( gsKit_texture_bmp(gsGlobal, gsFont->Texture, gsFont->Path_BMP) == -1 )
		{
			printf("Error uploading font bmp!\n");
			return -1;
		}
		int File = fioOpen(gsFont->Path_DAT, O_RDONLY);
		fioLseek(File, 0, SEEK_SET);
		gsFont->Additional=malloc( 0x100 );
	        if(fioRead(File, gsFont->Additional, 0x100) <= 0)
	        {
	                printf("Could not load font sizes: %s\n", gsFont->Path_DAT);
	                return -1;
	        }
	        fioClose(File);

		gsFont->HChars=16;
		gsFont->VChars=16;
		gsFont->CharWidth = gsFont->Texture->Width / 16;
		gsFont->CharHeight = gsFont->Texture->Height / 16;

		return 0;
	}

	return -1; //type unknown
}

void gsKit_font_print(GSGLOBAL *gsGlobal, GSFONT *gsFont, int X, int Y, int Z,
                      unsigned long color, char *String)
{
	u8 oldalpha = gsGlobal->PrimAlpha;
	gsGlobal->PrimAlpha=ALPHA_BLEND_ADD;

	int cx,cy,i,l;
	char c;
	cx=X;
	cy=Y;
	l=strlen( String );
	if( gsFont->Type == GSKIT_FTYPE_BMP_DAT ||
		gsFont->Type == GSKIT_FTYPE_FNT)
	{
		for( i=0;i<l;i++ )
		{
			c=String[i];
			if( c=='\n' )
			{
				cx=X;
				cy+=gsFont->CharHeight;
			}
			else
			{
				int px,py,charsiz;
				px=c%16;
				py=(c-px)/16;
				charsiz=gsFont->Additional[(u8)c];

				gsKit_prim_sprite_texture(gsGlobal, gsFont->Texture, cx, cy, 
					px*gsFont->CharWidth, py*gsFont->CharHeight, 
					cx+gsFont->CharWidth, cy+gsFont->CharHeight, 
					(px+1)*gsFont->CharWidth, (py+1)*gsFont->CharHeight, 
					Z, color);

				cx+=charsiz+1;
			}
		}

	}
	else
	{
		//font type unknown
	}
	
	gsGlobal->PrimAlpha=oldalpha;
}
