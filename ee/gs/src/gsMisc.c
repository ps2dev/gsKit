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

float gsKit_scale(GSGLOBAL gsGlobal, int axis, float vertex)
{
	float tempvertex;	

	if(axis == GS_AXIS_X)
	{
		if(gsGlobal.Aspect != GS_ASPECT_4_3)
		{
			tempvertex = vertex * (4095.9375 / gsGlobal.Width);
		}

		if(gsGlobal.Aspect != GS_ASPECT_16_9)
		{
			tempvertex = vertex * (4095.9375 / gsGlobal.Width);
		}
	}

	else if(axis == GS_AXIS_Y)
	{
		if(gsGlobal.Aspect != GS_ASPECT_4_3)
		{
			tempvertex = vertex * (4095.9375 / gsGlobal.Height);
		}

		if(gsGlobal.Aspect != GS_ASPECT_16_9)
		{
			tempvertex = vertex * (4095.9375 / gsGlobal.Height);
		}
	}

	return tempvertex;
}
