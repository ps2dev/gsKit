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

#include "gsKit.h"

u8 gsKit_font_upload(GSGLOBAL *gsGlobal, GSFONT *gsFont, GSTEXTURE *Texture)
{
	gsFont->Additional=0;

	if( gsFont->Type == GSKIT_FTYPE_FNT )
	{
		if( gsKit_texture_fnt(gsGlobal, gsFont, Texture) == -1 )
		{
			printf("Error uploading font!\n");
			return -1; 
		}
	}
	else if( gsFont->Type == GSKIT_FTYPE_BMP_DAT )
	{
		char tmp[512];
		sprintf( tmp, "%s.bmp", gsFont->Path );
		if( gsKit_texture_bmp(gsGlobal, Texture, tmp) == -1 )
		{
			printf("Error uploading font bmp!\n");
			return -1;
		}

		sprintf( tmp, "%s.dat", gsFont->Path );
		int File = fioOpen(tmp, O_RDONLY);
		fioLseek(File, 0, SEEK_SET);
		gsFont->Additional=malloc( 0x100 );
        if(fioRead(File, gsFont->Additional, 0x100) <= 0)
        {
                printf("Could not load font sizes: %s\n", tmp);
                return -1;
        }
        fioClose(File);

		gsFont->HChars=16;
		gsFont->VChars=16;
		gsFont->CharWidth=gsFont->Texture.Width/16;
		gsFont->CharHeight=gsFont->Texture.Height/16;

	}
	else return -1; //type unknown
}

void gsKit_font_print(GSGLOBAL *gsGlobal, GSFONT *gsFont, int X, int Y, int Z,
                      unsigned long color, char *String)
{
	gsGlobal->PrimAlpha=ALPHA_BLEND_ADD;

	int cx,cy,i,l;
	char c;
	cx=X;
	cy=Y;
	l=strlen( String );
	if( gsFont->Type == GSKIT_FTYPE_BMP_DAT )
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
				charsiz=gsFont->Additional[c];

				gsKit_prim_sprite_texture(gsGlobal, &gsFont->Texture, 
					cx,							cy, 
					px*gsFont->CharWidth,		py*gsFont->CharHeight, 
					cx+gsFont->CharWidth,		cy+gsFont->CharHeight, 
					(px+1)*gsFont->CharWidth,	(py+1)*gsFont->CharHeight, 
					Z, color);

				cx+=charsiz+1;
			}
		}

	}
	else return; //font type unknown
	gsGlobal->PrimAlpha=1;
}
