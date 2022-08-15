#  ____     ___ |    / _____ _____
# |  __    |    |___/    |     |
# |___| ___|    |    \ __|__   |     gsKit Open Source Project.
# ----------------------------------------------------------------------
# Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
# Licenced under Academic Free License version 2.0
# Review gsKit README & LICENSE files for further details.
#
# ee/gs/src/Makefile - gsKit gs makefile.
#

GS_CORE_OBJS = gsKit_vram_alloc.o gsKit_vram_clear.o gsKit_sync_flip.o gsKit_setactive.o \
	gsKit_finish.o gsKit_lock_buffer.o gsKit_unlock_buffer.o gsKit_lock_status.o gsKit_display_buffer.o \
	gsKit_switch_context.o gsKit_vsync_wait.o gsKit_vsync_nowait.o gsKit_get_field.o gsKit_hsync_wait.o \
	gsKit_add_vsync_handler.o gsKit_remove_vsync_handler.o gsKit_add_hsync_handler.o gsKit_remove_hsync_handler.o \
	gsKit_clear.o gsKit_set_scissor.o gsKit_set_test.o gsKit_set_clamp.o gsKit_set_primalpha.o \
	gsKit_set_texfilter.o gsKit_set_dither_matrix.o gsKit_set_dither.o gsKit_set_drawfield.o \
	gsKit_set_finish.o gsKit_queue_exec_real.o gsKit_queue_reset.o gsKit_queue_exec.o gsKit_alloc_ucab.o \
	gsKit_free_ucab.o gsKit_queue_init.o gsKit_queue_free.o gsKit_queue_set.o gsKit_mode_switch.o

GS_FONTM_OBJS = gsKit_init_fontm.o gsKit_free_fontm.o gsKit_fontm_upload.o gsKit_fontm_unpack.o \
	gsKit_fontm_unpack_raw.o gsKit_fontm_unpack_raw_1.o gsKit_fontm_print_scaled.o

GS_HIRES_OBJS = gsHiresIinternals.o _gsKit_create_passes.o gsKit_hires_sync.o gsKit_hires_flip.o gsKit_hires_prepare_bg.o \
	gsKit_hires_set_bg.o gsKit_hires_init_screen.o gsKit_hires_init_screen.o gsKit_hires_init_global.o \
	gsKit_hires_deinit_global.o

GS_INIT_OBJS = _gsInit_internal.o gsKit_check_rom.o gsKit_set_display_offset.o gsKit_init_screen.o \
	gsKit_init_global_custom.o gsKit_deinit_global.o


GS_MISC_OBJS = gsKit_setup_tbw.o gsKit_vram_dump.o

GS_TEXT_PRIMITIVE_OBJS = gsKit_prim_point.o gsKit_prim_line_3d.o gsKit_prim_line_goraud_3d.o \
	gsKit_prim_line_strip.o gsKit_prim_line_strip_3d.o gsKit_prim_sprite.o gsKit_prim_triangle_3d.o \
	gsKit_prim_triangle_strip.o gsKit_prim_triangle_strip_3d.o gsKit_prim_triangle_fan.o \
	gsKit_prim_triangle_fan_3d.o gsKit_prim_triangle_gouraud_3d.o gsKit_prim_quad_3d.o \
	gsKit_prim_quad_gouraud_3d.o

GS_TEXT_MANAGER_OBJS = gsTextManagerInternals.o gsKit_TexManager_init.o gsKit_TexManager_setmode.o \
	gsKit_TexManager_bind.o gsKit_TexManager_invalidate.o gsKit_TexManager_free.o gsKit_TexManager_nextFrame.o

GS_TEXTURE_OBJS = gsKit_texture_size_ee.o gsKit_texture_size.o gsKit_texture_to_psm16.o \
	gsKit_texture_send.o gsKit_texture_send_inline.o gsKit_texture_upload.o gsKit_prim_sprite_texture_3d.o \
	gsKit_prim_sprite_striped_texture_3d.o gsKit_prim_triangle_texture_3d.o \
	gsKit_prim_triangle_goraud_texture_3d.o gsKit_prim_triangle_strip_texture.o \
	gsKit_prim_triangle_strip_texture_3d.o gsKit_prim_triangle_fan_texture.o gsKit_prim_triangle_fan_texture_3d.o \
	gsKit_prim_quad_texture_3d.o gsKit_prim_quad_goraud_texture_3d.o

GS_VU1_OBJS = 

EE_OBJS = $(GS_CORE_OBJS) $(GS_FONTM_OBJS) $(GS_HIRES_OBJS) $(GS_INIT_OBJS) $(GS_MISC_OBJS) \
	$(GS_TEXT_PRIMITIVE_OBJS) $(GS_TEXT_MANAGER_OBJS) $(GS_TEXTURE_OBJS) $(GS_VU1_OBJS)

EE_CFLAGS += -Wno-unused

all: $(EE_OBJS)
	$(EE_AR) rcs $(LIBGSKIT) $(EE_OBJS)

clean:
	rm -f $(EE_OBJS)

install: all

$(GS_CORE_OBJS:%=$(EE_OBJS_DIR)%): $(EE_SRC_DIR)gsCore.c
	$(EE_C_COMPILE) -DF_$(*:$(EE_OBJS_DIR)%=%) $< -c -o $@

$(GS_FONTM_OBJS:%=$(EE_OBJS_DIR)%): $(EE_SRC_DIR)gsFontM.c
	$(EE_C_COMPILE) -DF_$(*:$(EE_OBJS_DIR)%=%) $< -c -o $@

$(GS_HIRES_OBJS:%=$(EE_OBJS_DIR)%): $(EE_SRC_DIR)gsHires.c
	$(EE_C_COMPILE) -DF_$(*:$(EE_OBJS_DIR)%=%) $< -c -o $@

$(GS_INIT_OBJS:%=$(EE_OBJS_DIR)%): $(EE_SRC_DIR)gsInit.c
	$(EE_C_COMPILE) -DF_$(*:$(EE_OBJS_DIR)%=%) $< -c -o $@

$(GS_TEXT_PRIMITIVE_OBJS:%=$(EE_OBJS_DIR)%): $(EE_SRC_DIR)gsPrimitive.c
	$(EE_C_COMPILE) -DF_$(*:$(EE_OBJS_DIR)%=%) $< -c -o $@

$(GS_MISC_OBJS:%=$(EE_OBJS_DIR)%): $(EE_SRC_DIR)gsMisc.c
	$(EE_C_COMPILE) -DF_$(*:$(EE_OBJS_DIR)%=%) $< -c -o $@

$(GS_TEXT_MANAGER_OBJS:%=$(EE_OBJS_DIR)%): $(EE_SRC_DIR)gsTexManager.c
	$(EE_C_COMPILE) -DF_$(*:$(EE_OBJS_DIR)%=%) $< -c -o $@

$(GS_TEXTURE_OBJS:%=$(EE_OBJS_DIR)%): $(EE_SRC_DIR)gsTexture.c
	$(EE_C_COMPILE) -DF_$(*:$(EE_OBJS_DIR)%=%) $< -c -o $@

$(GS_VU1_OBJS:%=$(EE_OBJS_DIR)%): $(EE_SRC_DIR)gsVU1.c
	$(EE_C_COMPILE) -DF_$(*:$(EE_OBJS_DIR)%=%) $< -c -o $@

include ../../../Makefile.pref
include ../../../Makefile.global

