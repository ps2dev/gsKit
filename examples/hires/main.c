#include <gsKit.h>
#include <gsToolkit.h>
#include <gsHires.h>
#include <gsInline.h>
#include <dmaKit.h>
#include <malloc.h>
#include <math3d.h>
#include <kernel.h>

#include <draw.h>
#include <draw3d.h>

#include "teapot.c"


#define HIRES_MODE
//#define TEX_BG
#define FHD_BG
#define DYNAMIC_DITHERING


VECTOR object_position = { 0.00f, 8.00f, 0.00f, 1.00f };
VECTOR object_rotation = { 2.70f, 0.00f, 0.00f, 1.00f };

VECTOR camera_position = { 0.00f, 0.00f,  80.00f, 1.00f };
VECTOR camera_rotation = { 0.00f, 0.00f,   0.00f, 1.00f };

int light_count = 4;
int iXOffset=0, iYOffset=0;

VECTOR light_direction[4] = {
	{  0.00f,  0.00f,  0.00f, 1.00f },
	{  1.00f,  0.00f, -1.00f, 1.00f },
	{  0.00f,  1.00f, -1.00f, 1.00f },
	{ -1.00f, -1.00f, -1.00f, 1.00f }
};

VECTOR light_colour[4] = {
	{ 0.00f, 0.00f, 0.00f, 1.00f },
	{ 0.60f, 1.00f, 0.60f, 1.00f },
	{ 0.30f, 0.30f, 0.30f, 1.00f },
	{ 0.50f, 0.50f, 0.50f, 1.00f }
};

int light_type[4] = {
	LIGHT_AMBIENT,
	LIGHT_DIRECTIONAL,
	LIGHT_DIRECTIONAL,
	LIGHT_DIRECTIONAL
};


int render(GSGLOBAL *gsGlobal)
{
#ifdef TEX_BG
	u64 TexCol = GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00);
	GSTEXTURE bigtex;
#endif
#ifdef FHD_BG
	GSTEXTURE fhdbg;
#endif
	int i;

	// Matrices to setup the 3D environment and camera
	MATRIX local_world;
	MATRIX local_light;
	MATRIX world_view;
	MATRIX view_screen;
	MATRIX local_screen;

	VECTOR *temp_normals;
	VECTOR *temp_lights;
	color_f_t *temp_colours;
	vertex_f_t *temp_vertices;

	xyz_t   *verts;
	color_t *colors;

	// Allocate calculation space.
	temp_normals  = (VECTOR     *)memalign(128, sizeof(VECTOR)     * vertex_count);
	temp_lights   = (VECTOR     *)memalign(128, sizeof(VECTOR)     * vertex_count);
	temp_colours  = (color_f_t  *)memalign(128, sizeof(color_f_t)  * vertex_count);
	temp_vertices = (vertex_f_t *)memalign(128, sizeof(vertex_f_t) * vertex_count);

	// Allocate register space.
	verts  = (xyz_t   *)memalign(128, sizeof(xyz_t)   * vertex_count);
	colors = (color_t *)memalign(128, sizeof(color_t) * vertex_count);

#ifdef TEX_BG
	bigtex.Filter = GS_FILTER_LINEAR;
	bigtex.Delayed = 1;
	gsKit_texture_jpeg(gsGlobal, &bigtex, "host:bigtex.jpg");
#endif

#ifdef FHD_BG
	fhdbg.Filter = GS_FILTER_LINEAR;
	fhdbg.Delayed = 1;
	fhdbg.Vram = GSKIT_ALLOC_ERROR;
	gsKit_texture_jpeg(gsGlobal, &fhdbg, "host:fhdbg.jpg");
	gsKit_hires_prepare_bg(gsGlobal, &fhdbg);
	gsKit_hires_set_bg(gsGlobal, &fhdbg);
#endif

	printf("VRAM used: %dKiB\n", gsGlobal->CurrentPointer / 1024);
	printf("VRAM free: %dKiB\n", 4096 - (gsGlobal->CurrentPointer / 1024));

	// Create the view_screen matrix.
	create_view_screen(view_screen, 16.0f/9.0f, -0.20f, 0.20f, -0.20f, 0.20f, 1.00f, 2000.00f);

	if (gsGlobal->ZBuffering == GS_SETTING_ON)
		gsKit_set_test(gsGlobal, GS_ZTEST_ON);

	// A,B,C,D,FIX = 0,1,0,1,0:
	// A = 0 = Cs (Color Source)
	// B = 1 = Cd (Color Destination)
	// C = 0 = As (Alpha Source)
	// D = 1 = Cd (Color Destination)
	// FIX = not used
	//
	// Resulting color = (A-B)*C+D = (Cs-Cd)*As+Cd
	// This will blend the source over the destination
	// Note:
	// - Alpha 0x00 = fully transparent
	// - Alpha 0x80 = fully visible
	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0, 1, 0, 1, 128), 0);
	gsGlobal->PrimAlphaEnable = GS_SETTING_OFF;
	gsGlobal->PrimAAEnable = GS_SETTING_ON;

	// The main loop...
	for (;;)
	{
		// Spin the teapot a bit.
		object_rotation[1] += 0.005f; while (object_rotation[1] > 3.14f) { object_rotation[1] -= 6.28f; }

		// Create the local_world matrix.
		create_local_world(local_world, object_position, object_rotation);

		// Create the local_light matrix.
		create_local_light(local_light, object_rotation);

		// Create the world_view matrix.
		create_world_view(world_view, camera_position, camera_rotation);

		// Create the local_screen matrix.
		create_local_screen(local_screen, local_world, world_view, view_screen);

		// Calculate the normal values.
		calculate_normals(temp_normals, vertex_count, normals, local_light);

		// Calculate the lighting values.
		calculate_lights(temp_lights, vertex_count, temp_normals, light_direction, light_colour, light_type, light_count);

		// Calculate the colour values after lighting.
		calculate_colours((VECTOR *)temp_colours, vertex_count, colours, temp_lights);

		// Calculate the vertex values.
		calculate_vertices((VECTOR *)temp_vertices, vertex_count, vertices, local_screen);

		// Convert floating point vertices to fixed point and translate to center of screen.
		draw_convert_xyz(verts, 2048, 2048, 16, vertex_count, temp_vertices);

		// Convert floating point colours to fixed point.
		draw_convert_rgbq(colors, vertex_count, temp_vertices, temp_colours, 0x80);

#ifndef HIRES_MODE
		// HIRES mode automatically clears the screen
		// So only clear the screen in normal mode
		gsKit_clear(gsGlobal, 0);
#endif

#ifdef DYNAMIC_DITHERING
		// Dithering:
		// The standard 4x4 dithering matrix creates static noise to eliminate banding.
		// This static noise is a little visible, and can be improved by changing the matrix every frame
		// Keep adding 5 to get the most noisy pattern possible:
		//   0, 5, 2, 7, 4, 1, 6, 3
		for(i = 0; i < 15; i++)
		    gsGlobal->DitherMatrix[i] = (gsGlobal->DitherMatrix[i] + 5) & 7;
		gsKit_set_dither_matrix(gsGlobal);
#endif

#ifdef TEX_BG
		gsKit_TexManager_bind(gsGlobal, &bigtex);
		gsKit_prim_sprite_texture(gsGlobal, &bigtex,
			0.0f,	// X1
			0.0f,	// Y1
			0.0f,	// U1
			0.0f,	// V1
			gsGlobal->Width,	// X2
			gsGlobal->Height,	// Y2
			bigtex.Width,		// U2
			bigtex.Height,		// V2
			2,
			TexCol);
#endif

		for (i = 0; i < points_count; i+=3) {
			float fX=gsGlobal->Width/2;
			float fY=gsGlobal->Height/2;
			gsKit_prim_triangle_gouraud_3d(gsGlobal
				, (temp_vertices[points[i+0]].x + 1.0f) * fX, (temp_vertices[points[i+0]].y + 1.0f) * fY, verts[points[i+0]].z
				, (temp_vertices[points[i+1]].x + 1.0f) * fX, (temp_vertices[points[i+1]].y + 1.0f) * fY, verts[points[i+1]].z
				, (temp_vertices[points[i+2]].x + 1.0f) * fX, (temp_vertices[points[i+2]].y + 1.0f) * fY, verts[points[i+2]].z
				, colors[points[i+0]].rgbaq, colors[points[i+1]].rgbaq, colors[points[i+2]].rgbaq);
		}
#ifdef HIRES_MODE
		gsKit_hires_sync(gsGlobal);
		gsKit_hires_flip(gsGlobal);
#else
		gsKit_queue_exec(gsGlobal);
		gsKit_sync_flip(gsGlobal);
#endif
		gsKit_TexManager_nextFrame(gsGlobal);
	}

	free(temp_normals);
	free(temp_lights);
	free(temp_colours);
	free(temp_vertices);
	free(verts);
	free(colors);

	return 0;

}

int main(int argc, char *argv[])
{
#ifdef HIRES_MODE
	GSGLOBAL *gsGlobal = gsKit_hires_init_global();
#else
	GSGLOBAL *gsGlobal = gsKit_init_global();
#endif
	int iPassCount;

#if 0
	gsGlobal->Mode = GS_MODE_NTSC;
	gsGlobal->Interlace = GS_INTERLACED;
	//gsGlobal->Field = GS_FIELD;
	gsGlobal->Field = GS_FRAME;
	gsGlobal->Width = 704;
	gsGlobal->Height = 464;
	iXOffset = -32;
	iYOffset = 8;
	iPassCount = 2;
#endif
#if 0
	gsGlobal->Mode = GS_MODE_PAL;
	gsGlobal->Interlace = GS_INTERLACED;
	//gsGlobal->Field = GS_FIELD;
	gsGlobal->Field = GS_FRAME;
	gsGlobal->Width = 704;
	gsGlobal->Height = 556;
	iXOffset = -10;
	iYOffset = 5;
	iPassCount = 3;
#endif
#if 0
	gsGlobal->Mode = GS_MODE_DTV_480P;
	gsGlobal->Interlace = GS_NONINTERLACED;
	gsGlobal->Field = GS_FRAME;
	gsGlobal->Width = 704;
	gsGlobal->Height = 462;
	iXOffset = -4;
	iYOffset = 3;
	iPassCount = 2;
#endif
#if 0
	gsGlobal->Mode = GS_MODE_DTV_576P;
	gsGlobal->Interlace = GS_NONINTERLACED;
	gsGlobal->Field = GS_FRAME;
	gsGlobal->Width = 704;
	gsGlobal->Height = 556;
	iXOffset = 0;
	iYOffset = 0;
	iPassCount = 3;
#endif
#if 0
	gsGlobal->Mode = GS_MODE_DTV_720P;
	gsGlobal->Interlace = GS_NONINTERLACED;
	gsGlobal->Field = GS_FRAME;
	gsGlobal->Width = 1280;
	gsGlobal->Height = 720;//704;
	iXOffset = 0;
	iYOffset = 0;
	iPassCount = 3;
#endif
#if 1
	gsGlobal->Mode = GS_MODE_DTV_1080I;
	gsGlobal->Interlace = GS_INTERLACED;
	//gsGlobal->Field = GS_FIELD;
	gsGlobal->Field = GS_FRAME;
	gsGlobal->Width  = 1920;
	gsGlobal->Height = 1080;
	iXOffset = 0;
	iYOffset = 0;
	iPassCount = 3;
#endif
#if 0
	gsGlobal->Mode = GS_MODE_VGA_1280_60;
	gsGlobal->Interlace = GS_NONINTERLACED;
	gsGlobal->Field = GS_FRAME;
	gsGlobal->Width = 1280;
	gsGlobal->Height = 1024;
	iXOffset = 0;
	iYOffset = 0;
	iPassCount = 4;
#endif

	if ((gsGlobal->Interlace == GS_INTERLACED) && (gsGlobal->Field == GS_FRAME))
		gsGlobal->Height /= 2;

	gsGlobal->PSM  = GS_PSM_CT16S;
	gsGlobal->PSMZ = GS_PSMZ_16S;

	gsGlobal->Dithering = GS_SETTING_ON;
	gsGlobal->DoubleBuffering = GS_SETTING_ON;
	gsGlobal->ZBuffering = GS_SETTING_ON;

	dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);

#ifdef HIRES_MODE
	gsKit_hires_init_screen(gsGlobal, iPassCount);
#else
	gsKit_init_screen(gsGlobal);
#endif
	gsKit_set_display_offset(gsGlobal, iXOffset, iYOffset);

	render(gsGlobal);

#ifdef HIRES_MODE
	gsKit_hires_deinit_global(gsGlobal);
#else
	gsKit_deinit_global(gsGlobal);
#endif

	return 0;
}
