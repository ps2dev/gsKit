//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2022 - Daniel Santos <danielsantos346@gmail.com>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// cube.c - Example demonstrating gsKit 3D with triangle list operation.
//

#include <kernel.h>
#include <stdlib.h>
#include <malloc.h>
#include <tamtypes.h>
#include <math3d.h>

#include <gsKit.h>
#include <gsInline.h>
#include <dmaKit.h>

#include <draw.h>
#include <draw3d.h>

#include "mesh_data.c"

static const u64 BLACK_RGBAQ   = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x80,0x00);

VECTOR object_position = { 0.00f, 0.00f, 0.00f, 1.00f };
VECTOR object_rotation = { 0.00f, 0.00f, 0.00f, 1.00f };

VECTOR camera_position = { 0.00f, 0.00f, 100.00f, 1.00f };
VECTOR camera_rotation = { 0.00f, 0.00f,   0.00f, 1.00f };

void flipScreen(GSGLOBAL* gsGlobal)
{	
	gsKit_queue_exec(gsGlobal);
	gsKit_sync_flip(gsGlobal);
}

GSGLOBAL* init_graphics()
{
	GSGLOBAL* gsGlobal = gsKit_init_global();

	gsGlobal->PrimAlphaEnable = GS_SETTING_ON;
	//gsGlobal->PrimAAEnable = GS_SETTING_ON;

	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0, 1, 0, 1, 0), 0);

	dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC, D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);
	dmaKit_chan_init(DMA_CHANNEL_GIF);

	gsKit_set_clamp(gsGlobal, GS_CMODE_REPEAT);

	gsKit_vram_clear(gsGlobal);

	gsKit_init_screen(gsGlobal);

	gsKit_mode_switch(gsGlobal, GS_ONESHOT);

	return gsGlobal;

}

int gsKit_convert_xyz(vertex_f_t *output, GSGLOBAL* gsGlobal, int count, vertex_f_t *vertices)
{

	int z;
	unsigned int max_z;

	switch(gsGlobal->PSMZ){
		case GS_PSMZ_32:
			z = 32;
			break;

		case GS_PSMZ_24:
			z = 24;
			break;

		case GS_PSMZ_16:
		case GS_PSMZ_16S:
			z = 16;
			break;
		
		default:
			return -1;
	}

	float center_x = gsGlobal->Width/2;
	float center_y = gsGlobal->Height/2;
	max_z = 1 << (z - 1);

	for (int i = 0; i < count; i++)
	{
		output[i].x = ((vertices[i].x + 1.0f) * center_x);
		output[i].y = ((vertices[i].y + 1.0f) * center_y);
		output[i].z = (unsigned int)((vertices[i].z + 1.0f) * max_z);
	}

	// End function.
	return 0;

}

int render(GSGLOBAL* gsGlobal)
{

	// Matrices to setup the 3D environment and camera
	MATRIX local_world;
	MATRIX world_view;
	MATRIX view_screen;
	MATRIX local_screen;

	VECTOR *temp_vertices;

	VECTOR   *verts;
	color_t *colors;

	GSPRIMPOINT *gs_vertices = (GSPRIMPOINT *)memalign(128, sizeof(GSPRIMPOINT) * points_count);

	VECTOR *c_verts = (VECTOR *)memalign(128, sizeof(VECTOR) * points_count);
	VECTOR* c_colours = (VECTOR *)memalign(128, sizeof(VECTOR) * points_count);


	for (int i = 0; i < points_count; i++)
	{
		c_verts[i][0] = vertices[points[i]][0];
		c_verts[i][1] = vertices[points[i]][1];
		c_verts[i][2] = vertices[points[i]][2];
		c_verts[i][3] = vertices[points[i]][3];

		c_colours[i][0] = colours[points[i]][0];
		c_colours[i][1] = colours[points[i]][1];
		c_colours[i][2] = colours[points[i]][2];
		c_colours[i][3] = colours[points[i]][3];
	}

	// Allocate calculation space.
	temp_vertices = memalign(128, sizeof(VECTOR) * points_count);

	// Allocate register space.
	verts  = memalign(128, sizeof(VECTOR) * points_count);
	colors = memalign(128, sizeof(color_t)  * points_count);

	// Create the view_screen matrix.
	create_view_screen(view_screen, 4.0f/3.0f, -0.5f, 0.5f, -0.5f, 0.5f, 1.00f, 2000.00f);

	if (gsGlobal->ZBuffering == GS_SETTING_ON)
		gsKit_set_test(gsGlobal, GS_ZTEST_ON);

	gsGlobal->PrimAAEnable = GS_SETTING_ON;

	// The main loop...
	for (;;)
	{

		// Spin the cube a bit.
		object_rotation[0] += 0.008f; //while (object_rotation[0] > 3.14f) { object_rotation[0] -= 6.28f; }
		object_rotation[1] += 0.012f; //while (object_rotation[1] > 3.14f) { object_rotation[1] -= 6.28f; }

		// Create the local_world matrix.
		create_local_world(local_world, object_position, object_rotation);

		// Create the world_view matrix.
		create_world_view(world_view, camera_position, camera_rotation);

		// Create the local_screen matrix.
		create_local_screen(local_screen, local_world, world_view, view_screen);

		// Calculate the vertex values.
		calculate_vertices(temp_vertices, points_count, c_verts, local_screen);

		// Convert floating point vertices to fixed point and translate to center of screen.
		gsKit_convert_xyz((vertex_f_t*)verts, gsGlobal, points_count, (vertex_f_t*)temp_vertices);

		// Convert floating point colours to fixed point.
		draw_convert_rgbq(colors, points_count, (vertex_f_t*)temp_vertices, (color_f_t*)c_colours, 0x80);

		for (int i = 0; i < points_count; i++)
		{
			gs_vertices[i].rgbaq = color_to_RGBAQ(colors[i].r, colors[i].g, colors[i].b, colors[i].a, 0.0f);
			gs_vertices[i].xyz2 = vertex_to_XYZ2(gsGlobal, verts[i][0], verts[i][1], verts[i][2]);
		}

		gsKit_clear(gsGlobal, BLACK_RGBAQ);

		gsKit_prim_list_triangle_gouraud_3d(gsGlobal, points_count, gs_vertices);

		flipScreen(gsGlobal);

	}

	return 0;

}

int main(int argc, char *argv[])
{

	// Init GS.
	GSGLOBAL* gsGlobal = init_graphics();

	// Render the cube
	render(gsGlobal);

	// Sleep
	SleepThread();

	// End program.
	return 0;

}
