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
#include <stdio.h>
#include <kernel.h>

#include "gsKit.h"
#include "dmaKit.h"
#include "malloc.h"

GSGLOBAL *gsGlobal;

u64 BlueTrans;
u64 RedTrans;

int vsync_max = 50;
float x = 10;
float y = 10;
float width = 150;
float height = 150;

float VHeight;
volatile int vsync_num = 0;
int frame_num = 0;

int vsync_callback(void)
{
    gsKit_display_buffer(gsGlobal); // working buffer gets displayed

    // we only have a single buffer so we need to unlock it
    // after it's done displaying
    if (gsGlobal->DoubleBuffering == GS_SETTING_OFF)
        gsKit_unlock_buffer(gsGlobal);

    vsync_num++;

    asm("sync.l");
    EIntr();

    return 0;
}

int render_frame()
{
    vsync_num = 0;

    while(vsync_num<vsync_max) {
        gsKit_queue_reset(gsGlobal->Os_Queue); // clear the queue for the new frame

        if( y <= 10  && (x + width) < (gsGlobal->Width - 10))
            x+=10;
        else if( (y + height)  <  (VHeight - 10) && (x + width) >= (gsGlobal->Width - 10) )
            y+=10;
        else if( (y + height) >=  (VHeight - 10) && x > 10 )
            x-=10;
        else if( y > 10 && x <= 10 )
            y-=10;

        // add two new sprites to queue
        gsKit_prim_sprite(gsGlobal, x, y, x + width, y + height, 4, BlueTrans);

        // RedTrans must be a oneshot for proper blending!
        gsKit_prim_sprite(gsGlobal, 100.0f, 100.0f, 200.0f, 200.0f, 5, RedTrans);

        gsKit_queue_exec(gsGlobal);

        frame_num++;

        if(!(*GS_CSR & 8))
            continue;
        else {
            gsKit_lock_buffer(gsGlobal); // lock working buffer for displaying
            gsKit_vsync_nowait(); // initiate vsync which activates interrupt handler

            // if double buffering is enabled
            // unlock the old displayed buffer to be the new working buffer
            if (gsGlobal->DoubleBuffering == GS_SETTING_ON) {
                gsKit_unlock_buffer(gsGlobal);
                gsKit_switch_context(gsGlobal);
            }
        }

        // blocks until there's a buffer available
        // but don't block in double buffered mode
        //if (gsGlobal->DoubleBuffering == GS_SETTING_OFF)
            while(gsKit_lock_status(gsGlobal) == GS_SETTING_ON);

    }

    return 0;
}

int main(void)
{
    int callback_id;

	u64 White, Black, Red, Green, Blue, GreenTrans, WhiteTrans;
    gsGlobal = gsKit_init_global();

    dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);
	dmaKit_chan_init(DMA_CHANNEL_FROMSPR);
	dmaKit_chan_init(DMA_CHANNEL_TOSPR);

    callback_id = gsKit_add_vsync_handler(&vsync_callback);

    printf("callback_id = %d\n", callback_id);

    // By default the gsKit_init_global() uses an autodetected interlaced field mode
    // To set a new mode set these five variables for the resolution desired and
    // mode desired

    // Some examples
    // Make sure that gsGlobal->Height is a factor of the mode's gsGlobal->DH

    //gsGlobal->Mode = GS_MODE_NTSC
    //gsGlobal->Interlace = GS_INTERLACED;
    //gsGlobal->Field = GS_FIELD;
    //gsGlobal->Width = 640;
    //gsGlobal->Height = 448;

    gsGlobal->Mode = GS_MODE_PAL;
    gsGlobal->Interlace = GS_INTERLACED;
	gsGlobal->Field = GS_FIELD;
    gsGlobal->Width = 640;
    gsGlobal->Height = 512;

    gsKit_init_screen(gsGlobal);

    //gsGlobal->Mode = GS_MODE_DTV_480P;
    //gsGlobal->Interlace = GS_NONINTERLACED;
    //gsGlobal->Field = GS_FRAME;
    //gsGlobal->Width = 720;
    //gsGlobal->Height = 480;

    //gsGlobal->Mode = GS_MODE_DTV_1080I;
    //gsGlobal->Interlace = GS_INTERLACED;
    //gsGlobal->Field = GS_FIELD;
    //gsGlobal->Width = 640;
    //gsGlobal->Height = 540;
    //gsGlobal->PSM = GS_PSM_CT16;
    //gsGlobal->PSMZ = GS_PSMZ_16;
    //gsGlobal->Dithering = GS_SETTING_ON;

    // A width of 640 would work as well
    // However a height of 720 doesn't seem to work well
    //gsGlobal->Mode = GS_MODE_DTV_720P;
    //gsGlobal->Interlace = GS_NONINTERLACED;
    //gsGlobal->Field = GS_FRAME;
    //gsGlobal->Width = 640;
    //gsGlobal->Height = 360;
    //gsGlobal->PSM = GS_PSM_CT16;
    //gsGlobal->PSMZ = GS_PSMZ_16;

	// You can use these to turn off Z/Double Buffering. They are on by default.
	// Modes with widths >= 1024 might not work with them on
    // gsGlobal->DoubleBuffering = GS_SETTING_OFF;
    // gsGlobal->ZBuffering = GS_SETTING_OFF;

	gsGlobal->PrimAlphaEnable = GS_SETTING_ON;

	VHeight = gsGlobal->Height;//600;//gsGlobal->Height;

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

	White = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x00,0x00);
	Black = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x00,0x00);
	Red = GS_SETREG_RGBAQ(0xFF,0x00,0x00,0x00,0x00);
	Green = GS_SETREG_RGBAQ(0x00,0xFF,0x00,0x00,0x00);
	Blue = GS_SETREG_RGBAQ(0x00,0x00,0xFF,0x00,0x00);

	BlueTrans = GS_SETREG_RGBAQ(0x00,0x00,0xFF,0x40,0x00);
	RedTrans = GS_SETREG_RGBAQ(0xFF,0x00,0x00,0x60,0x00);
	GreenTrans = GS_SETREG_RGBAQ(0x00,0xFF,0x00,0x50,0x00);
	WhiteTrans = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x50,0x00);

	gsKit_mode_switch(gsGlobal, GS_PERSISTENT);

	gsKit_clear(gsGlobal, White);

	gsKit_set_test(gsGlobal, GS_ZTEST_OFF);

	gsKit_prim_line_strip(gsGlobal, LineStripPtr, 6, 1, Black);

	gsKit_prim_triangle_strip(gsGlobal, TriStripPtr, 6, 1, Red);

	gsKit_prim_line(gsGlobal, 525.0f, 125.0f, 575.0f, 125.0f, 1, Black);
	gsKit_prim_line(gsGlobal, 550.0f, 150.0f, 600.0f, 100.0f, 1, Black);

	gsKit_prim_point(gsGlobal, 575.0f, 75.0f, 1, Black);
	gsKit_prim_point(gsGlobal, 600.0f, 100.0f, 1, Black);
	gsKit_prim_point(gsGlobal, 625.0f, 125.0f, 1, Black);

	gsKit_prim_quad(gsGlobal, 150.0f, 150.0f,
				   150.0f, 400.0f,
				   450.0f, 150.0f,
				   450.0f, 400.0f, 2, Green);

	gsKit_set_test(gsGlobal, GS_ZTEST_ON);

	gsKit_prim_triangle_fan(gsGlobal, TriFanPtr, 8, 5, Black);

	gsKit_prim_quad_gouraud(gsGlobal, 500.0f, 250.0f,
					   500.0f, 350.0f,
					   600.0f, 250.0f,
					   600.0f, 350.0f, 2,
					   Red, Green, Blue, Black);

	gsKit_prim_triangle_gouraud(gsGlobal, 280.0f, 200.0f,
					       280.0f, 350.0f,
					       180.0f, 350.0f, 5,
					       Blue, Red, White);

	gsKit_prim_triangle(gsGlobal, 300.0f, 200.0f, 300.0f, 350.0f, 400.0f, 350.0f, 3, Red);

	gsKit_prim_sprite(gsGlobal, 400.0f, 100.0f, 500.0f, 200.0f, 5, Red);

	gsKit_mode_switch(gsGlobal, GS_ONESHOT);


    render_frame();

    printf("We have managed to render %d frames in %d vsyncs in PAL mode\n", frame_num, vsync_num);

    gsGlobal->Mode = GS_MODE_NTSC;
    gsGlobal->Interlace = GS_INTERLACED;
    gsGlobal->Field = GS_FIELD;
    gsGlobal->Width = 640;
    gsGlobal->Height = 448;
    vsync_max = 60;
    frame_num = 0;

    gsKit_init_screen(gsGlobal);

    render_frame();

    printf("We have managed to render %d frames in %d vsyncs in NTSC mode\n", frame_num, vsync_num);


	return 0;
}
