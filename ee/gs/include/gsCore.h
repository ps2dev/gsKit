//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsCore.h - Header for gsCore.c
//
// Parts taken from emoon's BreakPoint Demo Library
//

#ifndef __GSCORE_H__
#define __GSCORE_H__

#include "gsKit.h"

/// GS VRAM "System" Allocation (Framebuffer, ZBuffer, etc)
#define GSKIT_ALLOC_SYSBUFFER 0x00
/// GS VRAM "User" Allocation (Texture, CLUT, etc)
#define GSKIT_ALLOC_USERBUFFER 0x01

/// Return this is VRAM allocation fails.
#define GSKIT_ALLOC_ERROR 0x00

/// GS GIF Data Transfer Blocksize
#define GS_GIF_BLOCKSIZE 0x7fff

/// VRAM Allcoate Blocksize (For System Buffers)(CRITICAL FOR VRAM ALIGNMENT)
#define GS_VRAM_BLOCKSIZE_8K 8192
/// VRAM Allcoate Blocksize (For User Textures)(CRITICAL FOR VRAM ALIGNMENT)
#define GS_VRAM_BLOCKSIZE_256 256
/// Texture Base Width Allocation Allignment (32/24/16 Bit Textures)
#define GS_VRAM_TBWALIGN 64
/// Texture Base Width Allocation Allignment (8/4 Bit CLUT Textures)
#define GS_VRAM_TBWALIGN_CLUT 128

/// Repeats ("Tiles") Texture Across the Surface.
#define GS_CMODE_REPEAT 0x00
/// Stretches Texture Across the Surface
#define GS_CMODE_CLAMP 0x01
/// Stretches Texture Across the Region Defined by MINU, MAXU, MINV, and MAXV
#define GS_CMODE_REGION_CLAMP 0x02
/// Repeats a Portion of the Texture Defined by UMSK (MINU), VMSK (MINV), UFIX (MAXU), and VFIX (MAXV)
#define GS_CMODE_REGION_REPEAT 0x03
/// Resets CLAMP vales to whatever is in gsGlobal->Clamp
#define GS_CMODE_RESET 0xFF

/// Turns off Z Testing
#define GS_ZTEST_OFF 0x01
/// Turns on Z Testing
#define GS_ZTEST_ON 0x02
/// Turns off Alpha Testing (Source)
#define GS_ATEST_OFF 0x03
/// Turns on Alpha Testing (Source)
#define GS_ATEST_ON 0x04
/// Turns off Alpha Testing (Destination)
#define GS_D_ATEST_OFF 0x05
/// Turns on Alpha Testing (Destination)
#define GS_D_ATEST_ON 0x06

/// Use simple filtering (nearest neighbor)
#define GS_FILTER_NEAREST 0x00
/// Use bilinear filter on texture
#define GS_FILTER_LINEAR  0x01

/// Basic X/Y/Z Vertex Structure
struct gsVertex
{
	s32 x __attribute__ ((packed));
	s32 y __attribute__ ((packed));
	s32 z __attribute__ ((packed));
};
typedef struct gsVertex GSVERTEX;

#ifdef __cplusplus
extern "C" {
#endif

/// GS VRAM Allocation
u32 gsKit_vram_alloc(GSGLOBAL *gsGlobal, u32 size, u8 type);

/// GS VRAM Clear
void gsKit_vram_clear(GSGLOBAL *gsGlobal);

/// Flips Framebuffers on VSync
/// This calls gsKit_vsync_wait, then calls gsKit_setactive
/// to swap your framebuffers.
///
/// It then sets your active PrimContext and ActiveBuffer
/// appropriately.
void gsKit_sync_flip(GSGLOBAL *gsGlobal);

/// Sets the LockBuffer parameter of gsGlobal to GS_SETTING_ON
/// Used with gsKit_unlock_buffer and gsKit_lock_status
/// This enables you to lock your current working buffer so it isn't
/// flipped while it's being worked on
void gsKit_lock_buffer(GSGLOBAL *gsGlobal);

/// Sets the LockBuffer parameter of gsGlobal to GS_SETTING_OFF
/// Used with gsKit_unlock_buffer and gsKit_lock_status
/// This enables you to unlock your current working buffer so it can
/// be flipped
void gsKit_unlock_buffer(GSGLOBAL *gsGlobal);

/// Returns the status of LockBuffer parameter of gsGlobal
/// If it returns GS_SETTING_ON then the buffer is locked
/// If it returns GS_SETTING_OFF then the buffer is unlocked
int gsKit_lock_status(GSGLOBAL *gsGlobal);

/// Switches the currently displayed buffer for the current
/// working buffer or front buffer with back buffer
void gsKit_display_buffer(GSGLOBAL *gsGlobal);

/// Switches the current working buffer with the
/// last displayed buffer or back buffer with front buffer
void gsKit_switch_context(GSGLOBAL *gsGlobal);

/// Sets Your Active Framebuffer
void gsKit_setactive(GSGLOBAL *gsGlobal);

/// Blocks until FINISH is triggered
void gsKit_finish(void);

/// Blocks Until a VSync
void gsKit_vsync_wait(void);

/// Blocks until a HSync
void gsKit_hsync_wait(void);

/// Initiates a vsync but doesn't block
void gsKit_vsync_nowait(void);

/// Installs a vsync interrupt handler (vblank_start)
int gsKit_add_vsync_handler(int (*vsync_callback)());

/// Removes a vsync interrupt handler
void gsKit_remove_vsync_handler(int callback_id);

/// Installs a hsync interrupt handler (hblank_start)
int gsKit_add_hsync_handler(int (*hsync_callback)());

/// Removes a hsync interrupt handler
void gsKit_remove_hsync_handler(int callback_id);

/// Sets gsGlobal->EvenOrOdd depending on current field drawing
void gsKit_get_field(GSGLOBAL *gsGlobal);

/// Sets the GS to draw only on even/odd/both fields (from libgraph)
void gsKit_set_drawfield(GSGLOBAL *gsGlobal, u8 field);

/// Clears the Screen With the Specified Color
/// This actually just momentarily turns off Z Test,
/// then draws a sprite with the given color at a Z depth of 0.
/// It then restores whatever your previous Z Test settings were.
void gsKit_clear(GSGLOBAL *gsGlobal, u64 Color);

/// Sets the TEST Parameters
/// This manipulates the TEST struture of gsGlobal according to
/// the Preset value. It then sends the new TEST parameters to the GS
/// to put it into effect.
///
/// Valid Presets Are:
///
/// GS_ZTEST_OFF - Turns off Z Testing
///
/// GS_ZTEST_ON - Turns on Z Testing
///
/// GS_ATEST_OFF - Turns off Alpha Testing (Source)
///
/// GS_ATEST_ON - Turns on Alpha Testing (Source)
///
/// GS_D_ATEST_OFF - Turns off Alpha Testing (Destination)
///
/// GS_D_ATEST_ON - Turns on Alpha Testing (Destination)
void gsKit_set_test(GSGLOBAL *gsGlobal, u8 Preset);

/// Sets the CLAMP Parameters
/// This manipulates the CLAMP struture of gsGlobal according to
/// the Preset value. It then sends the new CLAMP parameters to the GS
/// to put it into effect.
///
/// Valid Presets Are:
///
/// GS_CMODE_REPEAT - Repeats ("Tiles") Texture Across the Surface.
///
/// GS_CMODE_CLAMP - Stretches Texture Across the Surface
///
/// GS_CMODE_REGION_REPEAT - Stretches Texture Across the Region Defined
/// by MINU, MAXU, MINV, and MAXV
///
/// GS_CMODE_REGION_CLAMP - Repeats a Portion of the Texture Defined by
/// UMSK (MINU), VMSK (MINV), UFIX (MAXU), and VFIX (MAXV)
void gsKit_set_clamp(GSGLOBAL *gsGlobal, u8 Preset);

/// Sets the Alpha Blending Parameters
void gsKit_set_primalpha(GSGLOBAL *gsGlobal, u64 AlphaMode, u8 PerPixel);

/// Sets the Texture Filtering Parameters
void gsKit_set_texfilter(GSGLOBAL *gsGlobal, u8 FilterMode);

/// Sets the Dither Matrix Setting
void gsKit_set_dither_matrix(GSGLOBAL *gsGlobal);

/// Enables/Disables dithering, with the gsGlobal Dithering option.
void gsKit_set_dither(GSGLOBAL *gsGlobal);

/// Append the Current Queue with a GS_FINISH Register
GSQUEUE gsKit_set_finish(GSGLOBAL *gsGlobal);

/// Reset specified drawqueue to it's initial state (Useful for clearing the Persistent Queue)
void gsKit_queue_reset(GSQUEUE *Queue);

/// Normal User Draw Queue "Execution" (Kicks Oneshot and Persistent Queues)
void gsKit_queue_exec(GSGLOBAL *gsGlobal);

/// Specific Draw Queue "Execution" (Kicks the Queue passed for the second argument)
void gsKit_queue_exec_real(GSGLOBAL *gsGlobal, GSQUEUE *Queue);

/// Allocate UCAB buffer in GSQUEUE, internal use only
void *gsKit_alloc_ucab(int size);

/// Free UCAB buffer in GSQUEUE, internal use only
void gsKit_free_ucab(void *p);

/// Initialize a Draw Queue (Allocates memory for the Queue)
void gsKit_queue_init(GSGLOBAL *gsGlobal, GSQUEUE *Queue, u8 mode, int size);

/// Free Allocated Memory for Draw Queue
void gsKit_queue_free(GSGLOBAL *gsGlobal, GSQUEUE *Queue);

/// Set Current Draw Queue
void gsKit_queue_set(GSGLOBAL *gsGlobal, GSQUEUE *Queue);

/// Set Current Draw Queue (Between GS_ONESHOT and GS_PERSISTENT)
void gsKit_mode_switch(GSGLOBAL *gsGlobal, u8 mode);

#ifdef __cplusplus
}
#endif

#endif /* __GSCORE_H__ */
