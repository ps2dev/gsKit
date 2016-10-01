//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// textures.c - Example demonstrating gsKit texture operation.
//

#include <stdio.h>
#include <malloc.h>

#include <gsKit.h>
#include <dmaKit.h>
#include <gsToolkit.h>

int main(int argc, char *argv[])
{
    GSGLOBAL *gsGlobal = gsKit_init_global();
    GSTEXTURE Tex1, Tex2;

#ifdef HAVE_LIBJPEG
    GSTEXTURE Tex3;
#endif

    u64 White = GS_SETREG_RGBAQ(0xFF, 0xFF, 0xFF, 0x00, 0x00);
    u64 TexCol = GS_SETREG_RGBAQ(0x80, 0x80, 0x80, 0x80, 0x00);

    gsGlobal->PSM = GS_PSM_CT24;
    gsGlobal->PSMZ = GS_PSMZ_16S;
    // gsGlobal->DoubleBuffering = GS_SETTING_OFF;
    // gsGlobal->ZBuffering = GS_SETTING_OFF;

    dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
                D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);

    // Initialize the DMAC
    dmaKit_chan_init(DMA_CHANNEL_GIF);

    gsKit_init_screen(gsGlobal);

    gsKit_mode_switch(gsGlobal, GS_PERSISTENT);

    gsKit_clear(gsGlobal, White);

    Tex1.Width = 256;
    Tex1.Height = 256;
    Tex1.PSM = GS_PSM_CT24;
    Tex1.Filter = GS_FILTER_NEAREST;

    gsKit_texture_raw(gsGlobal, &Tex1, "host:bitmap.raw");
    printf("Texture 1 Height: %i\n", Tex1.Height);
    printf("Texture 1 Width: %i\n", Tex1.Width);

    gsKit_texture_bmp(gsGlobal, &Tex2, "host:bsdgirl.bmp");
    printf("Texture 2 Height: %i\n", Tex2.Height);
    printf("Texture 2 Width: %i\n", Tex2.Width);

    printf("Texure 1 VRAM Range = 0x%X - 0x%X\n", Tex1.Vram, Tex1.Vram + gsKit_texture_size(Tex1.Width, Tex1.Height, Tex1.PSM) - 1);
    printf("Texure 2 VRAM Range = 0x%X - 0x%X\n", Tex2.Vram, Tex2.Vram + gsKit_texture_size(Tex2.Width, Tex2.Height, Tex2.PSM) - 1);

#ifdef HAVE_LIBJPEG
    gsKit_texture_jpeg(gsGlobal, &Tex3, "host:ps2dev.jpg");

    printf("Texture 3 Height: %i\n", Tex3.Height);
    printf("Texture 3 Width: %i\n", Tex3.Width);
    printf("Texure 3 VRAM Range = 0x%X - 0x%X\n", Tex3.Vram, Tex3.Vram + gsKit_texture_size(Tex3.Width, Tex3.Height, Tex3.PSM) - 1);
#endif

    gsKit_set_clamp(gsGlobal, GS_CMODE_CLAMP);

    gsKit_clear(gsGlobal, White);

    gsKit_prim_quad_texture_3d(gsGlobal, &Tex1, 20.0f,  // X1
                               50.0f,                   // Y1
                               1,                       // Z1
                               0.0f,                    // U1
                               0.0f,                    // V1

                               20.0f,                // X2
                               50.0f + Tex1.Height,  // Y2
                               1,                    // Z2
                               0.0f,                 // U2
                               Tex1.Height,          // V2

                               20.0f + Tex1.Width,  // X3
                               50.0f,               // Y3
                               1,                   // Z3
                               Tex1.Width,          // U3
                               0.0f,                // V3

                               Tex1.Width + 20.0f,   // X4
                               Tex1.Height + 50.0f,  // Y4
                               1,                    // Z4
                               Tex1.Width,           // U4
                               Tex1.Height,          // V4
                               TexCol);

    gsKit_prim_sprite_texture(gsGlobal, &Tex2, 310.0f,  // X1
                              50.0f,                    // Y2
                              0.0f,                     // U1
                              0.0f,                     // V1
                              Tex2.Width + 310.0f,      // X2
                              Tex2.Height + 50.0f,      // Y2
                              Tex2.Width,               // U2
                              Tex2.Height,              // V2
                              2,
                              TexCol);


#ifdef HAVE_LIBJPEG
    gsKit_prim_sprite_texture(gsGlobal, &Tex3, 40.0f,  // X1
                              386.0f,                  // Y2
                              0.0f,                    // U1
                              0.0f,                    // V1
                              Tex3.Width + 40.0f,      // X2
                              Tex3.Height + 386.0f,    // Y2
                              Tex3.Width,              // U2
                              Tex3.Height,             // V2
                              3,
                              TexCol);
#endif

    while (1) {
        gsKit_sync_flip(gsGlobal);
        gsKit_queue_exec(gsGlobal);
    }

    return 0;
}
