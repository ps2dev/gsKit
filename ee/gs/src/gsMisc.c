//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsMisc.c - Miscellaneous Helper Routines
//

#include "gsKit.h"

int gsKit_scale(GSGLOBAL *gsGlobal, u8 axis, int vertex)
{
	if(axis == GS_AXIS_X)
	{
			vertex = vertex << 4;
	}

	else if(axis == GS_AXIS_Y)
	{
		vertex = vertex << 4;
		if(gsGlobal->Mode == GS_MODE_NTSC || gsGlobal->Mode == GS_MODE_PAL)
		{
			if( gsGlobal->Interlace == GS_NONINTERLACED )
				vertex = vertex / 2;
		}
	}

	return vertex;
}
