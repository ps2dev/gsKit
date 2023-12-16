#  ____     ___ |    / _____ _____
# |  __    |    |___/    |     |
# |___| ___|    |    \ __|__   |     gsKit Open Source Project.
# ----------------------------------------------------------------------
# Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
# Licenced under Academic Free License version 2.0
# Review gsKit README & LICENSE files for further details.

cmake_minimum_required(VERSION 3.0.0)

project(gsKit)

enable_language(ASM)

set(PS2DEV $ENV{PS2DEV})
if(${PS2DEV} STREQUAL "")
	message(FATAL_ERROR "Please kindly set $PS2DEV environment variable")
endif()

set(GSKIT $ENV{GSKIT})
if(${GSKIT} STREQUAL "")
	set(GSKIT ${PS2DEV}/gsKit)
endif()

set(INSTALL_LIB_DIR "${GSKIT}/lib" CACHE PATH "Installation directory for libraries")
set(INSTALL_INC_DIR "${GSKIT}/include" CACHE PATH "Installation directory for headers")

set(GSKIT_EXTERNAL_LIBS "")

find_library(LIBJPEG_LIBRARY NAMES jpeg)
find_path(LIBJPEG_INCLUDE_DIR NAMES jpeglib.h)

if(LIBJPEG_LIBRARY)
	list(APPEND GSKIT_EXTERNAL_LIBS ${LIBJPEG_LIBRARY})
	include_directories(${LIBJPEG_INCLUDE_DIR})
	add_definitions(-DHAVE_LIBJPEG)
endif()

find_library(LIBTIFF_LIBRARY NAMES tiff)
find_path(LIBTIFF_INCLUDE_DIR NAMES tiff.h)

if(LIBTIFF_LIBRARY)
	list(APPEND GSKIT_EXTERNAL_LIBS ${LIBTIFF_LIBRARY})
	include_directories(${LIBTIFF_INCLUDE_DIR})
	add_definitions(-DHAVE_LIBTIFF)
endif()

find_library(LIBZ_LIBRARY NAMES z)
find_path(LIBZ_INCLUDE_DIR NAMES zlib.h)

if(LIBZ_LIBRARY)
	list(APPEND GSKIT_EXTERNAL_LIBS ${LIBZ_LIBRARY})
	include_directories(${LIBZ_INCLUDE_DIR})
	add_definitions(-DHAVE_ZLIB)
endif()

find_library(LIBPNG_LIBRARY NAMES png)
find_path(LIBPNG_INCLUDE_DIR NAMES png.h)

if(LIBPNG_LIBRARY)
	list(APPEND GSKIT_EXTERNAL_LIBS ${LIBPNG_LIBRARY})
	include_directories(${LIBPNG_INCLUDE_DIR})
	add_definitions(-DHAVE_LIBPNG)
endif()

macro (add_object_library_macros objlistvar sourcefile)
	set(objects_list "")
	foreach(arg IN ITEMS ${ARGN})
		add_library(${objlistvar}_${arg} OBJECT ${sourcefile})
		target_compile_definitions(${objlistvar}_${arg} PRIVATE F_${arg}=1)
		list(APPEND objects_list $<TARGET_OBJECTS:${objlistvar}_${arg}>)
	endforeach()
	set(${objlistvar} ${objects_list})
endmacro (add_object_library_macros)

include_directories(ee/gs/include)

add_object_library_macros(GS_CORE_OBJS ee/gs/src/gsCore.c
	gsKit_vram_alloc
	gsKit_vram_clear
	gsKit_sync_flip
	gsKit_setactive
	gsKit_finish
	gsKit_lock_buffer
	gsKit_unlock_buffer
	gsKit_lock_status
	gsKit_display_buffer
	gsKit_switch_context
	gsKit_vsync_wait
	gsKit_vsync_nowait
	gsKit_get_field
	gsKit_hsync_wait
	gsKit_add_vsync_handler
	gsKit_remove_vsync_handler
	gsKit_add_hsync_handler
	gsKit_remove_hsync_handler
	gsKit_clear
	gsKit_set_scissor
	gsKit_set_test
	gsKit_set_clamp
	gsKit_set_primalpha
	gsKit_set_texfilter
	gsKit_set_dither_matrix
	gsKit_set_dither
	gsKit_set_drawfield
	gsKit_set_finish
	gsKit_queue_exec_real
	gsKit_queue_reset
	gsKit_queue_exec
	gsKit_alloc_ucab
	gsKit_free_ucab
	gsKit_queue_init
	gsKit_queue_free
	gsKit_queue_set
	gsKit_mode_switch
)
add_object_library_macros(GS_FONTM_OBJS ee/gs/src/gsFontM.c
	gsKit_init_fontm
	gsKit_free_fontm
	gsKit_fontm_upload
	gsKit_fontm_unpack
	gsKit_fontm_unpack_raw
	gsKit_fontm_unpack_raw_1
	gsKit_fontm_print_scaled
)
add_object_library_macros(GS_HIRES_OBJS ee/gs/src/gsHires.c
	gsHiresIinternals
	_gsKit_create_passes
	gsKit_hires_sync
	gsKit_hires_flip
	gsKit_hires_prepare_bg
	gsKit_hires_set_bg
	gsKit_hires_init_screen
	gsKit_hires_init_global
	gsKit_hires_deinit_global
)
add_object_library_macros(GS_INIT_OBJS ee/gs/src/gsInit.c
	_gsInit_internal
	gsKit_check_rom
	gsKit_set_display_offset
	gsKit_init_screen
	gsKit_init_global_custom
	gsKit_deinit_global
)
add_object_library_macros(GS_PRIMITIVE_OBJS ee/gs/src/gsPrimitive.c
	gsKit_prim_point
	gsKit_prim_list_points
	gsKit_prim_line_3d
	gsKit_prim_line_goraud_3d
	gsKit_prim_list_line_goraud_3d
	gsKit_prim_line_strip
	gsKit_prim_line_strip_3d
	gsKit_prim_sprite
	gsKit_prim_triangle_3d
	gsKit_prim_triangle_strip
	gsKit_prim_triangle_strip_3d
	gsKit_prim_triangle_fan
	gsKit_prim_triangle_fan_3d
	gsKit_prim_triangle_gouraud_3d
	gsKit_prim_list_triangle_gouraud_3d
	gsKit_prim_quad_3d
	gsKit_prim_quad_gouraud_3d
)
add_object_library_macros(GS_MISC_OBJS ee/gs/src/gsMisc.c
	gsKit_setup_tbw
	gsKit_vram_dump
)
add_object_library_macros(GS_TEXT_MANAGER_OBJS ee/gs/src/gsTexManager.c
	gsTextManagerInternals
	gsKit_TexManager_init
	gsKit_TexManager_setmode
	gsKit_TexManager_bind
	gsKit_TexManager_invalidate
	gsKit_TexManager_free
	gsKit_TexManager_nextFrame
)
add_object_library_macros(GS_TEXTURE_OBJS ee/gs/src/gsTexture.c
	gsKit_texture_size_ee
	gsKit_texture_size
	gsKit_texture_to_psm16
	gsKit_texture_send
	gsKit_texture_send_inline
	gsKit_texture_upload
	gsKit_prim_sprite_texture_3d
	gsKit_prim_sprite_striped_texture_3d
	gsKit_prim_triangle_texture_3d
	gskit_prim_list_sprite_texture_uv_3d
	gsKit_prim_triangle_goraud_texture_3d
	gsKit_prim_list_triangle_goraud_texture_uv_3d
	gsKit_prim_list_triangle_goraud_texture_stq_3d
	gsKit_prim_triangle_strip_texture
	gsKit_prim_triangle_strip_texture_3d
	gsKit_prim_triangle_fan_texture
	gsKit_prim_triangle_fan_texture_3d
	gsKit_prim_quad_texture_3d
	gsKit_prim_quad_goraud_texture_3d
)

add_library(gskit STATIC
	${GS_CORE_OBJS}
	${GS_FONTM_OBJS}
	${GS_HIRES_OBJS}
	${GS_INIT_OBJS}
	${GS_PRIMITIVE_OBJS}
	${GS_MISC_OBJS}
	${GS_TEXT_MANAGER_OBJS}
	${GS_TEXTURE_OBJS}
)

target_include_directories(gskit PUBLIC ee/gs/include)

include_directories(ee/dma/include)

add_object_library_macros(DMA_CORE_OBJS ee/dma/src/dmaCore.c
	dmaKit_wait
	dmaKit_wait_fast
	dmaKit_send
	dmaKit_send_ucab
	dmaKit_send_spr
	dmaKit_send_chain
	dmaKit_send_chain_ucab
	dmaKit_send_chain_spr
	dmaKit_get_spr
)
add_object_library_macros(DMA_INIT_OBJS ee/dma/src/dmaInit.c
	dmaKit_init
	dmaKit_chan_init
)
add_object_library_macros(DMA_SPR_OBJS ee/dma/src/dmaSpr.c
	dmaKit_spr_begin
	dmaKit_spr_end
	dmaKit_spr_alloc
)

add_library(dmakit STATIC
	${DMA_CORE_OBJS}
	${DMA_INIT_OBJS}
	${DMA_SPR_OBJS}
)

target_include_directories(dmakit PUBLIC ee/dma/include)

include_directories(ee/toolkit/include)

add_object_library_macros(GS_TOOLKIT_OBJS ee/toolkit/src/gsToolkit.c
	gsKit_texture_finish
	gsKit_texture_png
	gsKit_texture_bmp
	gsKit_texture_jpeg
	gsKit_texture_tiff
	gsKit_texture_raw
	gsKit_texture_tga
	gsKit_texture_fnt_raw
	gsKit_init_font
	gsKit_font_upload_raw
	gsKit_font_upload
	gsKit_font_print_scaled
)

add_library(gskit_toolkit STATIC
	${GS_TOOLKIT_OBJS}
)

target_include_directories(gskit_toolkit PUBLIC ee/toolkit/include)


if(NOT SKIP_INSTALL_LIBRARIES AND NOT SKIP_INSTALL_ALL)
	install(TARGETS gskit dmakit gskit_toolkit
		ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
		LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
	)
endif()
if(NOT SKIP_INSTALL_HEADERS AND NOT SKIP_INSTALL_ALL)
	install(DIRECTORY ee/gs/include/ ee/dma/include/ ee/toolkit/include/
		DESTINATION "${INSTALL_INC_DIR}"
		 FILES_MATCHING PATTERN "*.h"
	)
endif()

if(NOT SKIP_BUILD_EXAMPLES)
	set(CMAKE_EXECUTABLE_SUFFIX .elf)
	set(GSKIT_SAMPLE_LIBS "")
	list(APPEND GSKIT_SAMPLE_LIBS gskit dmakit gskit_toolkit ${GSKIT_EXTERNAL_LIBS})

	macro (add_executable_example targetname)
		add_executable(${targetname}
			examples/${targetname}/${targetname}.c
		)
		target_link_libraries(${targetname}
			${GSKIT_SAMPLE_LIBS}
		)
	endmacro (add_executable_example)

	set(GSKIT_SAMPLE_DIRS "")
	list(APPEND GSKIT_SAMPLE_DIRS
		alpha
		atlas
		basic
		bigtex
		coverflow
		clut
		cube
		fb
		fhdbg
		font
		fontm
		modetest
		modetesthires
		pixelperfect
		texstream
		textures
		vsync
	)

	foreach(arg IN ITEMS ${GSKIT_SAMPLE_DIRS})
		add_executable_example(${arg})
	endforeach()

	target_link_libraries(cube
		draw
		math3d
	)
	set_property(TARGET modetest APPEND PROPERTY SOURCES examples/modetest/pad.c)
	target_link_libraries(modetest
		pad
	)
	set_property(TARGET modetesthires APPEND PROPERTY SOURCES examples/modetesthires/pad.c)
	target_link_libraries(modetesthires
		pad
	)

	add_executable(hires
		examples/hires/main.c
	)
	target_link_libraries(hires
		${GSKIT_SAMPLE_LIBS}
		draw
		graph
		math3d
		packet
		dma
	)

	add_executable(linuz-texture
		examples/linuz-texture/sample.c
		examples/linuz-texture/testorig.s
		examples/linuz-texture/texture.c
	)
	target_link_libraries(linuz-texture
		${GSKIT_SAMPLE_LIBS}
	)

	add_executable(png-texture
		examples/png-texture/textures.c
	)
	target_link_libraries(png-texture
		${GSKIT_SAMPLE_LIBS}
	)
endif()
