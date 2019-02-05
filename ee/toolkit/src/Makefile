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

EE_OBJS = gsKit_texture_finish.o gsKit_texture_png.o gsKit_texture_bmp.o gsKit_texture_jpeg.o gsKit_texture_tiff.o gsKit_texture_raw.o gsKit_texture_tga.o \
	gsKit_texture_fnt_raw.o gsKit_init_font.o gsKit_font_upload_raw.o gsKit_font_upload.o gsKit_font_print_scaled.o

all: $(EE_OBJS)
	$(EE_AR) rcs $(LIBGSKIT_TOOLKIT) $(EE_OBJS)

clean:
	rm -f $(EE_OBJS)

install: all

$(EE_OBJS:%=$(EE_OBJS_DIR)%): $(EE_SRC_DIR)gsToolkit.c
	$(EE_C_COMPILE) -DF_$(*:$(EE_OBJS_DIR)%=%) $< -c -o $@

include ../../../Makefile.pref
include ../Makefile.global

