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

s8 gsKit_font_upload(GSGLOBAL *gsGlobal, GSFONT *gsFont)
{
	if( gsKit_texture_fnt(gsGlobal, gsFont) == -1 )
	{
		printf("Error uploading font!\n");
		return -1;
	}

	return 0;
}

void gsKit_font_print(GSGLOBAL *gsGlobal, GSFONT *gsFont, int X, int Y, int Z,
                      unsigned long color, char *String)
{

}


