//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsInit.c - GS initialization and configuration routines.
//
// Parts taken from emoon's BreakPoint Demo Library
//

#include "gsKit.h"

int gsKit_detect_signal(void)
{
	if (*(volatile char *)(0x1FC7FF52) == 'E')
	{
		/* PAL (SCEE) detected */
		return GS_MODE_PAL;
	}
	else
	{
		/* rest of the world is NTSC */
		return GS_MODE_NTSC;
	}
}

void gsKit_init_screen(GSGLOBAL *gsGlobal)
{
	u64	*p_data;
	u64	*p_store;
	u8 Mode = 0;
	int	fbHeight = 0;
	int	size;

	size = 17;

	if(!gsGlobal->Setup)
	{
		gsGlobal->CurrentPointer = 0;
		if(gsGlobal->Mode == GS_MODE_NTSC_I)
		{
			Mode = GS_MODE_NTSC;
			fbHeight = gsGlobal->Height / 2;
		}
		else if(gsGlobal->Mode == GS_MODE_PAL_I)
		{
			Mode = GS_MODE_PAL;
			fbHeight = (gsGlobal->Height / 2);
		}
		else if(gsGlobal->Mode == GS_MODE_DTV_1080I_I)
		{
			Mode = GS_MODE_DTV_1080I;
			fbHeight = (gsGlobal->Height / 2);
		}
		else
		{
			fbHeight = gsGlobal->Height;
			Mode = gsGlobal->Mode;
		}
		
		gsGlobal->CurrentPointer = 0;
	
		GS_RESET();

		__asm__("sync.p; nop;");

		GsPutIMR(0x0000F700);
		
		SetGsCrt(gsGlobal->Interlace, Mode, gsGlobal->Field);

		gsGlobal->Setup = 1;
	}

	if(gsGlobal->ZBuffering == GS_SETTING_OFF)
	{
		gsGlobal->Test->ZTE = GS_SETTING_ON;
		gsGlobal->Test->ZTST = 1;
	}
	
	GS_SET_PMODE(	0,		// Read Circuit 1
			1,		// Read Circuit 2
			0,		// Use ALP Register for Alpha Blending
			1,		// Alpha Value of Read Circuit 2 for Output Selection
			0,		// Blend Alpha with output of Read Circuit 2
			0x80);		// Alpha Value = 1.0

	GS_SET_DISPFB1(	0,			// Frame Buffer Base Pointer (Address/2048)
			gsGlobal->Width / 64,	// Buffer Width (Address/64)
			gsGlobal->PSM,		// Pixel Storage Format
			0,			// Upper Left X in Buffer
			0);	

	GS_SET_DISPFB2(	0,			// Frame Buffer Base Pointer (Address/2048)
			gsGlobal->Width / 64,	// Buffer Width (Address/64)
			gsGlobal->PSM,		// Pixel Storage Format
			0,			// Upper Left X in Buffer
			0);			// Upper Left Y in Buffer

	GS_SET_DISPLAY1(gsGlobal->StartX,		// X position in the display area (in VCK unit
			gsGlobal->StartY,		// Y position in the display area (in Raster u
			gsGlobal->MagX,			// Horizontal Magnification
			gsGlobal->MagY,			// Vertical Magnification
			(gsGlobal->Width * 4) -1,	// Display area width
			(gsGlobal->Height-1));		// Display area height

	GS_SET_DISPLAY2(gsGlobal->StartX,		// X position in the display area (in VCK units)
			gsGlobal->StartY,		// Y position in the display area (in Raster units)
			gsGlobal->MagX,			// Horizontal Magnification
			gsGlobal->MagY,			// Vertical Magnification
			(gsGlobal->Width * 4) -1,	// Display area width
			(gsGlobal->Height-1));		// Display area height
	
	GS_SET_BGCOLOR(	gsGlobal->BGColor->Red,		// Red
			gsGlobal->BGColor->Green,	// Green
			gsGlobal->BGColor->Blue);	// Blue

	gsGlobal->CurrentPointer = 0;
	// Context 1
	gsGlobal->ScreenBuffer[0] = gsKit_vram_alloc( gsGlobal, gsKit_texture_size(gsGlobal->Width, fbHeight, gsGlobal->PSM), GSKIT_ALLOC_SYSBUFFER );
	if(gsGlobal->DoubleBuffering == GS_SETTING_OFF)
	{
		gsGlobal->ScreenBuffer[1] = gsGlobal->ScreenBuffer[0];
	}
	else
		// Context 2
		gsGlobal->ScreenBuffer[1] = gsKit_vram_alloc( gsGlobal, gsKit_texture_size(gsGlobal->Width, fbHeight, gsGlobal->PSM), GSKIT_ALLOC_SYSBUFFER );
	
	if(gsGlobal->ZBuffering == GS_SETTING_ON)
		gsGlobal->ZBuffer = gsKit_vram_alloc( gsGlobal, gsKit_texture_size(gsGlobal->Width, fbHeight, gsGlobal->PSMZ), GSKIT_ALLOC_SYSBUFFER ); // Z Buffer

	(u32)p_data = (u32)p_store  = gsGlobal->dma_misc;

	*p_data++ = GIF_TAG( size - 1, 1, 0, 0, 0, 1 );
	*p_data++ = GIF_AD;

	*p_data++ = 1;
	*p_data++ = GS_PRMODECONT;
	
	*p_data++ = GS_SETREG_FRAME_1( gsGlobal->ScreenBuffer[0], gsGlobal->Width / 64, gsGlobal->PSM, 0 );
	*p_data++ = GS_FRAME_1;

	*p_data++ = GS_SETREG_XYOFFSET_1( gsGlobal->Offset, 
					  gsGlobal->Offset);
	*p_data++ = GS_XYOFFSET_1;
					  
	*p_data++ = GS_SETREG_SCISSOR_1( 0, gsGlobal->Width - 1, 0, gsGlobal->Height - 1 );
	*p_data++ = GS_SCISSOR_1;

	*p_data++ = GS_SETREG_TEST( gsGlobal->Test->ATE, gsGlobal->Test->ATST, 
				gsGlobal->Test->AREF, gsGlobal->Test->AFAIL,
				gsGlobal->Test->DATE, gsGlobal->Test->DATM,
				gsGlobal->Test->ZTE, gsGlobal->Test->ZTST );
	
	*p_data++ = GS_TEST_1;
	
	*p_data++ = GS_SETREG_CLAMP(gsGlobal->Clamp->WMS, gsGlobal->Clamp->WMT, 
				gsGlobal->Clamp->MINU, gsGlobal->Clamp->MAXU, 
				gsGlobal->Clamp->MINV, gsGlobal->Clamp->MAXV);

	*p_data++ = GS_CLAMP_1;

	if(gsGlobal->ZBuffering == GS_SETTING_ON)
	{
		*p_data++ = GS_SETREG_ZBUF_1( gsGlobal->ZBuffer / 8192, gsGlobal->PSMZ, 0 );
		*p_data++ = GS_ZBUF_1;
	}
	if(gsGlobal->ZBuffering == GS_SETTING_OFF)
	{
		*p_data++ = GS_SETREG_ZBUF_1( NULL, gsGlobal->PSM, 1 );
		*p_data++ = GS_ZBUF_1;
	}

	*p_data++ = GS_SETREG_COLCLAMP( 255 );
	*p_data++ = GS_COLCLAMP;

	*p_data++ = GS_SETREG_FRAME_1( gsGlobal->ScreenBuffer[1], gsGlobal->Width / 64, gsGlobal->PSM, 0 );
	*p_data++ = GS_FRAME_2;

	*p_data++ = GS_SETREG_XYOFFSET_1( gsGlobal->Offset, 
					  gsGlobal->Offset);
	*p_data++ = GS_XYOFFSET_2;	
					  
	*p_data++ = GS_SETREG_SCISSOR_1( 0, gsGlobal->Width - 1, 0, gsGlobal->Height - 1);
	*p_data++ = GS_SCISSOR_2;

	*p_data++ = GS_SETREG_TEST( gsGlobal->Test->ATE, gsGlobal->Test->ATST, 
				gsGlobal->Test->AREF, gsGlobal->Test->AFAIL,
				gsGlobal->Test->DATE, gsGlobal->Test->DATM,
				gsGlobal->Test->ZTE, gsGlobal->Test->ZTST );
	
	*p_data++ = GS_TEST_2;
	
	*p_data++ = GS_SETREG_CLAMP(gsGlobal->Clamp->WMS, gsGlobal->Clamp->WMT, 
				gsGlobal->Clamp->MINU, gsGlobal->Clamp->MAXU, 
				gsGlobal->Clamp->MINV, gsGlobal->Clamp->MAXV);

	*p_data++ = GS_CLAMP_2;

	if(gsGlobal->ZBuffering == GS_SETTING_ON)
	{
		*p_data++ = GS_SETREG_ZBUF_1( gsGlobal->ZBuffer / 8192, gsGlobal->PSMZ, 0 );
		*p_data++ = GS_ZBUF_2;
	}
	if(gsGlobal->ZBuffering == GS_SETTING_OFF)
	{
		*p_data++ = GS_SETREG_ZBUF_1( NULL, gsGlobal->PSM, 1 );
		*p_data++ = GS_ZBUF_2;
	}

	*p_data++ = GS_BLEND_BACK2FRONT;
	*p_data++ = GS_ALPHA_1;

	*p_data++ = GS_BLEND_BACK2FRONT;
	*p_data++ = GS_ALPHA_2;

	dmaKit_send_ucab(DMA_CHANNEL_GIF, p_store, size);
	dmaKit_wait_fast(DMA_CHANNEL_GIF);
}

GSGLOBAL *gsKit_init_global_custom(u8 mode, int Os_AllocSize, int Per_AllocSize)
{
	GSGLOBAL *gsGlobal = calloc(1,sizeof(GSGLOBAL));
	gsGlobal->BGColor = calloc(1,sizeof(GSBGCOLOR));	
	gsGlobal->Test = calloc(1,sizeof(GSTEST));
	gsGlobal->Clamp = calloc(1,sizeof(GSCLAMP));
	gsGlobal->Os_Queue = calloc(1,sizeof(GSQUEUE));
	gsGlobal->Per_Queue = calloc(1,sizeof(GSQUEUE));
	(u32)gsGlobal->dma_misc = ((u32)memalign(64, 512) | 0x30000000);

	/* Generic Values */
	gsGlobal->Setup = 0;
	gsGlobal->Aspect = GS_ASPECT_4_3;

        gsGlobal->PSM = GS_PSM_CT24;
        gsGlobal->PSMZ = GS_PSMZ_32;

	gsGlobal->DoSubOffset = GS_SETTING_OFF;

	gsGlobal->DrawOrder = GS_PER_OS;

	gsGlobal->EvenOrOdd = 1;

	gsGlobal->Os_AllocSize = Os_AllocSize;
	(u32)gsGlobal->Os_Queue->dma_tag = (u32)gsGlobal->Os_Queue->pool[0] = ((u32)memalign(64, Os_AllocSize) | 0x30000000);
	(u32)gsGlobal->Os_Queue->pool[1] = ((u32)memalign(64, Os_AllocSize) | 0x30000000);
	(u32)gsGlobal->Os_Queue->pool_cur = ((u32)gsGlobal->Os_Queue->pool[0] + 16);
	(u32)gsGlobal->Os_Queue->pool_max[0] = ((u32)gsGlobal->Os_Queue->pool[0] + Os_AllocSize);
	(u32)gsGlobal->Os_Queue->pool_max[1] = ((u32)gsGlobal->Os_Queue->pool[1] + Os_AllocSize);
	gsGlobal->Os_Queue->dbuf = 0;
	gsGlobal->Os_Queue->tag_size = 0;
	gsGlobal->Os_Queue->last_tag = gsGlobal->Os_Queue->pool_cur;
	gsGlobal->Os_Queue->last_type = GIF_RESERVED;
	gsGlobal->Os_Queue->mode = GS_ONESHOT;
	gsGlobal->Per_AllocSize = Per_AllocSize;
	(u32)gsGlobal->Per_Queue->dma_tag = (u32)gsGlobal->Per_Queue->pool[0] = ((u32)memalign(64, Per_AllocSize) | 0x30000000);
	(u32)gsGlobal->Per_Queue->pool_cur = ((u32)gsGlobal->Per_Queue->pool[0] + 16);
	(u32)gsGlobal->Per_Queue->pool_max[0] = ((u32)gsGlobal->Per_Queue->pool[0] + Per_AllocSize);
	gsGlobal->Per_Queue->dbuf = 0;
	gsGlobal->Per_Queue->tag_size = 0;
	gsGlobal->Per_Queue->last_tag = gsGlobal->Per_Queue->pool_cur;
	gsGlobal->Per_Queue->last_type = GIF_RESERVED;
	gsGlobal->Per_Queue->mode = GS_PERSISTENT;

	gsGlobal->CurQueue = gsGlobal->Os_Queue;

	/* Auto-detect signal if needed */
	if(mode == GS_MODE_AUTO)
	{
		mode = gsKit_detect_signal();
	}
	else if(mode == GS_MODE_AUTO_I)
	{
		int m = gsKit_detect_signal();
		mode = (m == GS_MODE_NTSC) ? GS_MODE_NTSC_I : GS_MODE_PAL_I;
	}

	if(mode == GS_MODE_NTSC)
	{
		gsGlobal->Interlace = GS_INTERLACED;
		gsGlobal->Field = GS_FIELD;
		gsGlobal->DoubleBuffering = GS_SETTING_ON;
		gsGlobal->ZBuffering = GS_SETTING_ON;
		gsGlobal->Mode = mode;
		gsGlobal->Width = 640;	
		gsGlobal->Height = 448;
		gsGlobal->StartX = 632;
		gsGlobal->StartY = 40;
		gsGlobal->MagX = 3;
		gsGlobal->MagY = 0;
	}
	else if(mode == GS_MODE_NTSC_I)
	{
		gsGlobal->Interlace = GS_INTERLACED;
		gsGlobal->Field = GS_FRAME;
		gsGlobal->DoubleBuffering = GS_SETTING_ON;
		gsGlobal->ZBuffering = GS_SETTING_ON;
		gsGlobal->Mode = mode;
		gsGlobal->Width = 640;
		gsGlobal->Height = 448; // really 224 per frame
		gsGlobal->StartX = 652;
		gsGlobal->StartY = 30;
		gsGlobal->MagX = 3;
		gsGlobal->MagY = 0;
		gsGlobal->DoSubOffset = GS_SETTING_ON;
	}
	else if(mode == GS_MODE_PAL)
	{
		gsGlobal->Interlace = GS_INTERLACED;
		gsGlobal->Field = GS_FIELD;
		gsGlobal->DoubleBuffering = GS_SETTING_ON;
		gsGlobal->ZBuffering = GS_SETTING_ON;
		gsGlobal->Mode = mode;
		gsGlobal->Width = 640;
		gsGlobal->Height = 512;
		gsGlobal->StartX = 680;
		gsGlobal->StartY = 50;
		gsGlobal->MagX = 3;
		gsGlobal->MagY = 0;
	}
	else if(mode == GS_MODE_PAL_I)
	{
		gsGlobal->Interlace = GS_INTERLACED;
		gsGlobal->Field = GS_FRAME;
		gsGlobal->DoubleBuffering = GS_SETTING_ON;
		gsGlobal->ZBuffering = GS_SETTING_ON;
		gsGlobal->Mode = mode;
		gsGlobal->Width = 640;		
		gsGlobal->Height = 512; // really 256 per frame
		gsGlobal->StartX = 680;
		gsGlobal->StartY = 50;
		gsGlobal->MagX = 3;
		gsGlobal->MagY = 0;
		gsGlobal->DoSubOffset = GS_SETTING_ON;
	}
	else if(mode == GS_MODE_VGA_640_60 || mode == GS_MODE_VGA_640_72 ||
			mode == GS_MODE_VGA_640_75 || mode == GS_MODE_VGA_640_85)
	{
		gsGlobal->Interlace = GS_NONINTERLACED;
		gsGlobal->Field = GS_FIELD;
		gsGlobal->DoubleBuffering = GS_SETTING_ON;
		gsGlobal->ZBuffering = GS_SETTING_ON;
		gsGlobal->Mode = mode;
		gsGlobal->Width = 640;
		gsGlobal->Height = 480;
		if(mode == GS_MODE_VGA_640_60)
			gsGlobal->StartX = 280;
		else if(mode == GS_MODE_VGA_640_72)
			gsGlobal->StartX = 330;
		else if(mode == GS_MODE_VGA_640_75)
			gsGlobal->StartX = 360;
		else
			gsGlobal->StartX = 260;
		gsGlobal->StartY = 18;
		gsGlobal->MagX = 1;
		gsGlobal->MagY = 0;
	}
	else if(mode == GS_MODE_VGA_800_56 || mode == GS_MODE_VGA_800_60 || 
			mode == GS_MODE_VGA_800_72 || mode == GS_MODE_VGA_800_75 || 
			mode == GS_MODE_VGA_800_85)
	{
		gsGlobal->Interlace = GS_NONINTERLACED;
		gsGlobal->Field = GS_FIELD;
		gsGlobal->DoubleBuffering = GS_SETTING_ON;
		gsGlobal->ZBuffering = GS_SETTING_ON;
		gsGlobal->Mode = mode;
		gsGlobal->Width = 800;
		gsGlobal->Height = 600;
		if(mode == GS_MODE_VGA_800_56)
			gsGlobal->StartX = 450;
		else if(mode == GS_MODE_VGA_800_60)
			gsGlobal->StartX = 465;
		else if(mode == GS_MODE_VGA_800_72)
			gsGlobal->StartX = 465;
		else if(mode == GS_MODE_VGA_800_75)
			gsGlobal->StartX = 510;
		else
			gsGlobal->StartX = 500;
		gsGlobal->StartY = 25;
		gsGlobal->MagX = 1;
		gsGlobal->MagY = 0;
	}
	else if(mode == GS_MODE_VGA_1024_60 || mode == GS_MODE_VGA_1024_70 ||
			mode == GS_MODE_VGA_1024_75 || mode == GS_MODE_VGA_1024_85)
	{
		gsGlobal->Interlace = GS_NONINTERLACED;
		gsGlobal->Field = GS_FIELD;
		gsGlobal->DoubleBuffering = GS_SETTING_OFF;
		gsGlobal->ZBuffering = GS_SETTING_OFF;
		gsGlobal->Mode = mode;
		gsGlobal->Width = 1024;
		gsGlobal->Height = 768;
		gsGlobal->StartY = 30;
		gsGlobal->MagY = 0;
		if(mode == GS_MODE_VGA_1024_60)
		{
			gsGlobal->MagX = 1;
			gsGlobal->StartX = 580;
		}
		else if(mode == GS_MODE_VGA_1024_70)
		{
			gsGlobal->MagX = 0;
			gsGlobal->StartX = 266;
		}
		else if(mode == GS_MODE_VGA_1024_75)
		{
			gsGlobal->MagX = 0;
			gsGlobal->StartX = 260;
		}
		else
		{
				gsGlobal->MagX = 0;
				gsGlobal->StartX = 290;
		}
	}
	else if(mode == GS_MODE_VGA_1280_60 || mode == GS_MODE_VGA_1280_75 )
	{
		gsGlobal->Interlace = GS_NONINTERLACED;
		gsGlobal->Field = GS_FIELD;
		gsGlobal->DoubleBuffering = GS_SETTING_OFF;
		gsGlobal->ZBuffering = GS_SETTING_OFF;
		gsGlobal->Mode = mode;
		gsGlobal->Width = 1280;
		gsGlobal->Height = 1024;
		gsGlobal->StartX = 350;
		gsGlobal->StartY = 40;
		gsGlobal->MagX = 0;
		gsGlobal->MagY = 0;
	}
	else if(mode == GS_MODE_DTV_480P)
	{
		gsGlobal->Interlace = GS_NONINTERLACED;
		gsGlobal->Field = GS_FIELD;
		gsGlobal->DoubleBuffering = GS_SETTING_ON;
		gsGlobal->ZBuffering = GS_SETTING_ON;
		gsGlobal->Mode = mode;
		gsGlobal->Width = 720;
		gsGlobal->Height = 480;
		gsGlobal->StartX = 232;
		gsGlobal->StartY = 35;
		gsGlobal->MagX = 1;
		gsGlobal->MagY = 0;
	}
	else if(mode == GS_MODE_DTV_720P)
	{
		gsGlobal->Interlace = GS_NONINTERLACED;
		gsGlobal->Field = GS_FIELD;
		gsGlobal->DoubleBuffering = GS_SETTING_OFF;
		gsGlobal->ZBuffering = GS_SETTING_OFF;
		gsGlobal->Mode = mode;
		gsGlobal->Width = 1280;
		gsGlobal->Height = 720;
		gsGlobal->StartX = 420;
		gsGlobal->StartY = 40;
		gsGlobal->MagX = 0;
		gsGlobal->MagY = 0;
	}
	else if(mode == GS_MODE_DTV_1080I)
	{
		gsGlobal->Interlace = GS_INTERLACED;
		gsGlobal->Field = GS_FIELD;
	        gsGlobal->PSM = GS_PSM_CT16;
		gsGlobal->DoubleBuffering = GS_SETTING_OFF;
		gsGlobal->ZBuffering = GS_SETTING_OFF;
		gsGlobal->Mode = mode;
		gsGlobal->Width = 1920;
		gsGlobal->Height = 1080;
		gsGlobal->StartX = 300;
		gsGlobal->StartY = 120;
		gsGlobal->MagX = 0;
		gsGlobal->MagY = 0;
	}
        else if(mode == GS_MODE_DTV_1080I_I)
        {
                gsGlobal->Interlace = GS_INTERLACED;
                gsGlobal->Field = GS_FRAME;
	        gsGlobal->PSM = GS_PSM_CT16;
                gsGlobal->DoubleBuffering = GS_SETTING_OFF;
                gsGlobal->ZBuffering = GS_SETTING_OFF;
                gsGlobal->Mode = mode;
                gsGlobal->Width = 1920;
                gsGlobal->Height = 1080;
                gsGlobal->StartX = 300;
                gsGlobal->StartY = 60;
                gsGlobal->MagX = 0;
                gsGlobal->MagY = 1;
        }
	else 
	{
		printf("Invalid Mode Selected, Defaulting to NTSC.\n");
		gsGlobal->Field = GS_FIELD;
		gsGlobal->Mode = GS_MODE_NTSC;
		gsGlobal->Width = 640;
		gsGlobal->Height = 448;
		gsGlobal->StartX = 632;
		gsGlobal->StartY = 50;
		gsGlobal->MagX = 3;
		gsGlobal->MagY = 0;
		gsGlobal->DoSubOffset = GS_SETTING_ON;
	}		
	gsGlobal->Offset = 2048 << 4;
	gsGlobal->ActiveBuffer = 1;
	gsGlobal->PrimFogEnable = GS_SETTING_OFF;
	gsGlobal->PrimAAEnable = GS_SETTING_OFF;
	gsGlobal->PrimAlphaEnable = GS_SETTING_OFF;
	gsGlobal->PrimAlpha = GS_BLEND_BACK2FRONT;
	gsGlobal->PrimContext = 0;
	gsGlobal->FirstFrame = GS_SETTING_ON;

	/* BGColor Register Values */
	gsGlobal->BGColor->Red = 0x00;
	gsGlobal->BGColor->Green = 0x00;
	gsGlobal->BGColor->Blue = 0x00;

	/* TEST Register Values */
	gsGlobal->Test->ATE = GS_SETTING_OFF;
	gsGlobal->Test->ATST = GS_SETTING_ON;
	gsGlobal->Test->AREF = 0x80;
	gsGlobal->Test->AFAIL = 0;
	gsGlobal->Test->DATE = GS_SETTING_OFF;
	gsGlobal->Test->DATM = 0;
	gsGlobal->Test->ZTE = GS_SETTING_ON;
	gsGlobal->Test->ZTST = 2;

	gsGlobal->Clamp->WMS = GS_CMODE_CLAMP;
	gsGlobal->Clamp->WMT = GS_CMODE_CLAMP;
	gsGlobal->Clamp->MINU = 0;
	gsGlobal->Clamp->MAXU = 0;
	gsGlobal->Clamp->MINV = 0;
	gsGlobal->Clamp->MAXV = 0;

	return gsGlobal;
}

GSFONT *gsKit_init_font(u8 type, char *path)
{
	
	GSFONT *gsFont = calloc(1,sizeof(GSFONT));
	gsFont->Texture = calloc(1,sizeof(GSTEXTURE));
	if(path)
	{
		gsFont->Path = calloc(1,strlen(path));
		strcpy(gsFont->Path, path);
	}
	gsFont->Type = type;
		
	return gsFont;
}

GSFONT *gsKit_init_font_raw(u8 type, u8 *data, int size)
{
	
	GSFONT *gsFont = calloc(1,sizeof(GSFONT));
	gsFont->Texture = calloc(1,sizeof(GSTEXTURE));
	gsFont->RawData = data;
	gsFont->RawSize = size;
	gsFont->Type = type;
		
	return gsFont;
}

