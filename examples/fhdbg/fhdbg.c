#include <stdio.h>

#include <dmaKit.h>
#include <gsKit.h>
#include <gsToolkit.h>

#define TEXTURE_COUNT 3

int main(int argc, char* argv[])
{
    GSGLOBAL* gsGlobal;
    GSTEXTURE Tex[TEXTURE_COUNT];
    int iPassCount;
    int iTexId;
    char filename[80];
    int iFrameCount = 0;

    gsGlobal = gsKit_init_global();

	printf("\n");
	printf("--------------------------------------------------------------------------------\n");
	printf("------------------------------- !!! WARNING !!! --------------------------------\n");
	printf("--------------------------------------------------------------------------------\n");
	printf("-                                                                              -\n");
	printf("- This example uses 'HIRES':                                                   -\n");
	printf("-                                                                              -\n");
	printf("- HIRES saves VRAM by having only the part of the frame                        -\n");
	printf("- in VRAM that is currently being read by the CRTC.                            -\n");
	printf("-                                                                              -\n");
	printf("- Use it on a REAL PS2!                                                        -\n");
	printf("- On an emulator (PCSX2), you will only see part of the screen.                -\n");
	printf("-                                                                              -\n");
	printf("--------------------------------------------------------------------------------\n");
	printf("\n");

    gsGlobal->Mode      = GS_MODE_DTV_1080I;
    gsGlobal->Interlace = GS_INTERLACED;
    gsGlobal->Field     = GS_FRAME;
    gsGlobal->Width     = 1920;
    gsGlobal->Height    = 540;
    iPassCount          = 3;

    gsGlobal->PSM             = GS_PSM_CT16S;
    gsGlobal->PSMZ            = GS_PSMZ_16S;
    gsGlobal->Dithering       = GS_SETTING_ON;
    gsGlobal->DoubleBuffering = GS_SETTING_ON;
    gsGlobal->ZBuffering      = GS_SETTING_OFF;

    dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
        D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);

    // Initialize the DMAC
    dmaKit_chan_init(DMA_CHANNEL_GIF);

    gsKit_hires_init_screen(gsGlobal, iPassCount);

    // Load textures
    for (iTexId = 0; iTexId < TEXTURE_COUNT; iTexId++) {
        Tex[iTexId].Delayed = 1;
        snprintf(filename, 80, "host:images/fhdbg_0%d.jpg", iTexId + 1);
        gsKit_texture_jpeg(gsGlobal, &Tex[iTexId], filename);
        gsKit_hires_prepare_bg(gsGlobal, &Tex[iTexId]);
    }

    iTexId = 0;
    gsKit_hires_set_bg(gsGlobal, &Tex[iTexId]);

    while (1) {
        // Wait for vsync
        gsKit_hires_sync(gsGlobal);

        // Next texture after 60 frames (1 second)
        iFrameCount++;
        if (iFrameCount >= 60) {
            iFrameCount = 0;
            iTexId++;
            if (iTexId >= TEXTURE_COUNT)
                iTexId = 0;
            gsKit_hires_set_bg(gsGlobal, &Tex[iTexId]);
            gsKit_hires_flip(gsGlobal);
        }
    }

    return 0;
}
