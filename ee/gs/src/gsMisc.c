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

int gsKit_scale(GSGLOBAL *gsGlobal, u8 axis, float vertex)
{
	int result = 0;

	if(axis == GS_AXIS_X || axis == GS_MAP_U || axis == GS_MAP_V)
	{			
		result = (int)(vertex * 16.0f);
		result += gsGlobal->OffsetX << 4;
	}
	else if(axis == GS_AXIS_Y)
	{
		vertex = vertex / 2;
				
		result = (int)(vertex * 16.0f);
		result += gsGlobal->OffsetY << 4;
	}
	else if(axis == GS_AXIS_Z)
	{
		result = (int)(vertex * 16.0f);
	}

	return result;
}
