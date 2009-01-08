//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsToolkit.c - Utility library for gsKit
//

#include <stdio.h>
#include <stdlib.h>

#include "gsKit.h"
#include "gsToolkit.h"

#ifdef HAVE_LIBJPG
#include <libjpg.h>
#endif

#ifdef HAVE_LIBTIFF
#include <tif_config.h>
#include <tiffio.h>
#endif

#ifdef HAVE_LIBPNG
#include <png.h>
#include <assert.h>

void gsKit_png_read(png_structp png_ptr, png_bytep data, png_size_t length)
{
	FILE* File = (FILE*) png_ptr->io_ptr;
	if(fread(data, length, 1, File) <= 0)
	{
		png_error(png_ptr, "Error reading via fread\n");
		return;
	}
}
#endif

int gsKit_texture_png(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, char *Path)
{
#ifdef HAVE_LIBPNG
	FILE* File = fopen(Path, "r");
	if (File == NULL)
	{
		printf("Failed to load PNG file: %s\n", Path);
		return -1;
	}

	png_structp png_ptr;
	png_infop info_ptr;
	png_uint_32 width, height;
	png_bytep *row_pointers;

	unsigned int sig_read = 0;
        int row, i, k=0, j, bit_depth, color_type, interlace_type;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp) NULL, NULL, NULL);

	if(!png_ptr)
	{
		printf("PNG Read Struct Init Failed\n");
		fclose(File);
		return -1;
	}

	info_ptr = png_create_info_struct(png_ptr);

	if(!info_ptr)
	{
		printf("PNG Info Struct Init Failed\n");
		fclose(File);
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		return -1;
	}

	if(setjmp(png_ptr->jmpbuf))
	{
		printf("Got PNG Error!\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(File);
		return -1;
	}

	png_set_read_fn(png_ptr, File, gsKit_png_read);

	png_set_sig_bytes(png_ptr, sig_read);

	png_read_info(png_ptr, info_ptr);

	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,&interlace_type, NULL, NULL);

	png_set_strip_16(png_ptr);

	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_expand(png_ptr);

	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand(png_ptr);

	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);

	png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);

	png_read_update_info(png_ptr, info_ptr);

	Texture->Width = width;
	Texture->Height = height;

        Texture->VramClut = 0;
        Texture->Clut = NULL;

	if(png_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA)
	{
        int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
		Texture->PSM = GS_PSM_CT32;
		Texture->Mem = memalign(128, gsKit_texture_size_ee(Texture->Width, Texture->Height, Texture->PSM));

        row_pointers = calloc(height, sizeof(png_bytep));

        for (row = 0; row < height; row++) {
            row_pointers[row] = malloc(row_bytes);
        }

        png_read_image(png_ptr, row_pointers);

        struct pixel { unsigned char r,g,b,a; };
        struct pixel *Pixels = (struct pixel *) Texture->Mem;

        for (i=0;i<height;i++) {
                for (j=0;j<width;j++) {
                        Pixels[k].r = row_pointers[i][4*j];
                        Pixels[k].g = row_pointers[i][4*j+1];
                        Pixels[k].b = row_pointers[i][4*j+2];
                        Pixels[k++].a = (int) row_pointers[i][4*j+3] * 128 / 255;
            }
		}

        for (row = 0; row < height; row++) {
            free(row_pointers[row]);
        }

        free(row_pointers);
		Texture->Filter = GS_FILTER_NEAREST;

		png_read_end(png_ptr, NULL);

		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
	}
	else if(png_ptr->color_type == PNG_COLOR_TYPE_RGB)
	{
        int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
		Texture->PSM = GS_PSM_CT24;
		Texture->Mem = memalign(128, gsKit_texture_size_ee(Texture->Width, Texture->Height, Texture->PSM));

        row_pointers = calloc(height, sizeof(png_bytep));

        for (row = 0; row < height; row++) {
            row_pointers[row] = malloc(row_bytes);
        }

        png_read_image(png_ptr, row_pointers);

        struct pixel3 { unsigned char r,g,b; };
        struct pixel3 *Pixels = (struct pixel3 *) Texture->Mem;

        for (i=0;i<height;i++) {
                for (j=0;j<width;j++) {
                        Pixels[k].r = row_pointers[i][4*j];
                        Pixels[k].g = row_pointers[i][4*j+1];
                        Pixels[k++].b = row_pointers[i][4*j+2];
            }
		}

        for (row = 0; row < height; row++) {
            free(row_pointers[row]);
        }

        free(row_pointers);
		Texture->Filter = GS_FILTER_NEAREST;

		png_read_end(png_ptr, NULL);

		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
	}
	else
	{
		printf("This texture depth is not supported yet!\n");
	}

	fclose(File);

	if(!Texture->Delayed)
	{
		u32 VramTextureSize = gsKit_texture_size(Texture->Width, Texture->Height, Texture->PSM);
		Texture->Vram = gsKit_vram_alloc(gsGlobal, VramTextureSize, GSKIT_ALLOC_USERBUFFER);
		if(Texture->Vram == GSKIT_ALLOC_ERROR)
		{
			printf("VRAM Allocation Failed. Will not upload texture.\n");
			return -1;
		}

		gsKit_texture_upload(gsGlobal, Texture);
		free(Texture->Mem);
	}
	else
	{
		gsKit_setup_tbw(Texture);
	}

	return 0;
#else
	printf("ERROR: gsKit_texture_png unimplemented.\n");
	return -1;
#endif
}

int gsKit_texture_bmp(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, char *Path)
{
	GSBITMAP Bitmap;
	int x, y;
	int cy;
	u32 FTexSize;
	u8  *image;
	u8  *p;

	FILE* File = fopen(Path, "r");
	if (File == NULL)
	{
		printf("Failed to load bitmap: %s\n", Path);
		return -1;
	}
	if (fread(&Bitmap.FileHeader, sizeof(Bitmap.FileHeader), 1, File) <= 0)
	{
		printf("Could not load bitmap: %s\n", Path);
		fclose(File);
		return -1;
	}

	if (fread(&Bitmap.InfoHeader, sizeof(Bitmap.InfoHeader), 1, File) <= 0)
	{
		printf("Could not load bitmap: %s\n", Path);
		fclose(File);
		return -1;
	}

	Texture->Width = Bitmap.InfoHeader.Width;
	Texture->Height = Bitmap.InfoHeader.Height;
	Texture->Filter = GS_FILTER_NEAREST;

	if(Bitmap.InfoHeader.BitCount == 4)
	{
		Texture->PSM = GS_PSM_T4;
		Texture->Clut = memalign(128, gsKit_texture_size_ee(8, 2, GS_PSM_CT32));
		Texture->ClutPSM = GS_PSM_CT32;

		memset(Texture->Clut, 0, gsKit_texture_size_ee(8, 2, GS_PSM_CT32));
		Texture->VramClut = gsKit_vram_alloc(gsGlobal, gsKit_texture_size(8, 2, GS_PSM_CT32), GSKIT_ALLOC_USERBUFFER);
		fseek(File, 54, SEEK_SET);
		if (fread(Texture->Clut, Bitmap.InfoHeader.ColorUsed*sizeof(u32), 1, File) <= 0)
		{
			printf("Could not load bitmap: %s\n", Path);
			fclose(File);
			return -1;
		}

		GSBMCLUT *clut = (GSBMCLUT *)Texture->Clut;
		int i;
		for (i = Bitmap.InfoHeader.ColorUsed; i < 16; i++)
		{
			memset(&clut[i], 0, sizeof(clut[i]));
		}

		for (i = 0; i < 16; i++)
		{
			u8 tmp = clut[i].Blue;
			clut[i].Blue = clut[i].Red;
			clut[i].Red = tmp;
			clut[i].Alpha = 0;
		}

	}
	else if(Bitmap.InfoHeader.BitCount == 8)
{
		Texture->PSM = GS_PSM_T8;
		Texture->Clut = memalign(128, gsKit_texture_size_ee(16, 16, GS_PSM_CT32));
		Texture->ClutPSM = GS_PSM_CT32;

		memset(Texture->Clut, 0, gsKit_texture_size_ee(16, 16, GS_PSM_CT32));
		Texture->VramClut = gsKit_vram_alloc(gsGlobal, gsKit_texture_size(16, 16, GS_PSM_CT32), GSKIT_ALLOC_USERBUFFER);
		fseek(File, 54, SEEK_SET);
		if (fread(Texture->Clut, Bitmap.InfoHeader.ColorUsed*sizeof(u32), 1, File) <= 0)
		{
			printf("Could not load bitmap: %s\n", Path);
			fclose(File);
			return -1;
		}

		GSBMCLUT *clut = (GSBMCLUT *)Texture->Clut;
		int i;
		for (i = Bitmap.InfoHeader.ColorUsed; i < 256; i++)
		{
			memset(&clut[i], 0, sizeof(clut[i]));
		}

		for (i = 0; i < 256; i++)
		{
			u8 tmp = clut[i].Blue;
			clut[i].Blue = clut[i].Red;
			clut[i].Red = tmp;
			clut[i].Alpha = 0;
		}

		// rotate clut
		for (i = 0; i < 256; i++)
		{
			if ((i&0x18) == 8)
			{
				GSBMCLUT tmp = clut[i];
				clut[i] = clut[i+8];
				clut[i+8] = tmp;
			}
		}
	}
	else if(Bitmap.InfoHeader.BitCount == 16)
{
		Texture->PSM = GS_PSM_CT16;
		Texture->VramClut = 0;
		Texture->Clut = NULL;
	}
	else if(Bitmap.InfoHeader.BitCount == 24)
	{
		Texture->PSM = GS_PSM_CT24;
		Texture->VramClut = 0;
		Texture->Clut = NULL;
	}

	FTexSize = fseek(File, 0, SEEK_END);
	FTexSize -= Bitmap.FileHeader.Offset;

	fseek(File, Bitmap.FileHeader.Offset, SEEK_SET);

	u32 TextureSize = gsKit_texture_size_ee(Texture->Width, Texture->Height, Texture->PSM);
	u32 VramTextureSize = gsKit_texture_size(Texture->Width, Texture->Height, Texture->PSM);

	Texture->Mem = memalign(128,TextureSize);

	if(Bitmap.InfoHeader.BitCount == 24)
	{
		image = memalign(128, FTexSize);
		if (image == NULL) return -1;
		fread(image, FTexSize, 1, File);
		p = (void *)((u32)Texture->Mem | 0x30000000);
		for (y=Texture->Height-1,cy=0; y>=0; y--,cy++) {
			for (x=0; x<Texture->Width; x++) {
				p[(y*Texture->Width+x)*3+2] = image[(cy*Texture->Width+x)*3+0];
				p[(y*Texture->Width+x)*3+1] = image[(cy*Texture->Width+x)*3+1];
				p[(y*Texture->Width+x)*3+0] = image[(cy*Texture->Width+x)*3+2];
			}
		}
		free(image);
	}
	else if(Bitmap.InfoHeader.BitCount == 16)
	{
		printf("16-bit BMP not supported yet.\n");
	}
	else if(Bitmap.InfoHeader.BitCount == 8 || Bitmap.InfoHeader.BitCount == 4 )
	{
		char *tex = (char *)((u32)Texture->Mem | 0x30000000);
		image = memalign(128,FTexSize);
		if (fread(image, FTexSize, 1, File) != 1)
		{
			printf("Read failed!\n");
			printf("Size %d\n", FTexSize);
			free(image);
			fclose(File);
		}
		for (y=Texture->Height-1; y>=0; y--)
		{
			if(Bitmap.InfoHeader.BitCount == 8)
				memcpy(&tex[y*Texture->Width], &image[(Texture->Height-y-1)*Texture->Width], Texture->Width);
			else
				memcpy(&tex[y*(Texture->Width/2)], &image[(Texture->Height-y-1)*(Texture->Width/2)], Texture->Width / 2);
		}
		free(image);

		if(Bitmap.InfoHeader.BitCount == 4)
		{
			int byte;
			u8 *tmpdst = (u8 *)((u32)Texture->Mem | 0x30000000);
			u8 *tmpsrc = (u8 *)tex;

			for(byte = 0; byte < FTexSize; byte++)
			{
				tmpdst[byte] = (tmpsrc[byte] << 4) | (tmpsrc[byte] >> 4);
			}
			free(tex);
		}
	}
	else
	{
		printf("Unknown BMP bit depth format %d\n", Bitmap.InfoHeader.BitCount);
	}

	fclose(File);

	if(!Texture->Delayed)
	{
		Texture->Vram = gsKit_vram_alloc(gsGlobal, VramTextureSize, GSKIT_ALLOC_USERBUFFER);
		if(Texture->Vram == GSKIT_ALLOC_ERROR)
		{
			printf("VRAM Allocation Failed. Will not upload texture.\n");
			return -1;
		}

		gsKit_texture_upload(gsGlobal, Texture);
		free(Texture->Mem);
	}
	else
	{
		gsKit_setup_tbw(Texture);
	}

	return 0;
}

int  gsKit_texture_jpeg(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, char *Path)
{
#ifdef HAVE_LIBJPG

	jpgData *jpg;

	int TextureSize = 0;
	int VramTextureSize = 0;

	jpg = jpgOpen(Path);
	if (jpg == NULL) {
		return -1;
		printf("error opening %s\n", Path);
	}

	Texture->Width =  jpg->width;
	Texture->Height = jpg->height;
	Texture->PSM = GS_PSM_CT24;
	Texture->Filter = GS_FILTER_NEAREST;
	Texture->VramClut = 0;
	Texture->Clut = NULL;

	TextureSize = gsKit_texture_size_ee(Texture->Width, Texture->Height, Texture->PSM);
	VramTextureSize = gsKit_texture_size(Texture->Width, Texture->Height, Texture->PSM);
	#ifdef DEBUG
	printf("Texture Size = %i\n",TextureSize);
	#endif

	Texture->Mem = memalign(128,TextureSize);
	jpgReadImage(jpg, (void *)Texture->Mem);
	jpgClose(jpg);


	if(!Texture->Delayed)
	{
		Texture->Vram = gsKit_vram_alloc(gsGlobal, VramTextureSize, GSKIT_ALLOC_USERBUFFER);
		if(Texture->Vram == GSKIT_ALLOC_ERROR)
		{
			printf("VRAM Allocation Failed. Will not upload texture.\n");
			return -1;
		}
		gsKit_texture_upload(gsGlobal, Texture);

		free(Texture->Mem);
	}
	else
	{
		gsKit_setup_tbw(Texture);
	}

	return 0;

#else

	printf("ERROR: gsKit_texture_jpeg unimplemented.\n");
	return -1;

#endif
}

int  gsKit_texture_tiff(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, char *Path)
{
#ifdef HAVE_LIBTIFF
	int TextureSize = 0;
	int VramTextureSize = 0;

	TIFF* tif = TIFFOpen(Path, "r");
	if(tif == NULL)
	{
		printf("Error Opening %s\n", Path);
		return -1;
	}

	Texture->PSM = GS_PSM_CT32;
	Texture->Filter = GS_FILTER_NEAREST;
	Texture->VramClut = 0;
	Texture->Clut = NULL;

	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &Texture->Width);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &Texture->Height);
	TextureSize = gsKit_texture_size_ee(Texture->Width, Texture->Height, Texture->PSM);
	VramTextureSize = gsKit_texture_size(Texture->Width, Texture->Height, Texture->PSM);

	#ifdef GSKIT_DEBUG
	printf("Texture Size = %i\n",TextureSize);
	#endif

	Texture->Mem = memalign(128,TextureSize);

	if (!TIFFReadPS2Image(tif, Texture->Width, Texture->Height, Texture->Mem, 0))
	{
		printf("Error Reading TIFF Data\n");
		TIFFClose(tif);
		free(Texture->Mem);
		return -1;
	}

	TIFFClose(tif);

/*
	// To dump+debug the RBGA data from tiff
	FILE* File = fopen("host:texdump.raw", "w");

	fwrite(Texture->Mem, TextureSize, 1, File);

	fclose(File);
*/
	if(!Texture->Delayed)
	{
		Texture->Vram = gsKit_vram_alloc(gsGlobal, VramTextureSize, GSKIT_ALLOC_USERBUFFER);
		if(Texture->Vram == GSKIT_ALLOC_ERROR)
		{
			printf("VRAM Allocation Failed. Will not upload texture.\n");
			return -1;
		}
		gsKit_texture_upload(gsGlobal, Texture);

		free(Texture->Mem);
	}
	else
	{
		gsKit_setup_tbw(Texture);
	}

	return 0;

#else

	printf("ERROR: gsKit_texture_tiff unimplimented.\n");
	return -1;

#endif
}

int gsKit_texture_raw(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, char *Path)
{
	FILE* File = fopen(Path, "r");
	if (File == NULL)
	{
		printf("Failed to load  texture: %s\n", Path);
		return -1;
	}
	int FileSize = gsKit_texture_size_ee(Texture->Width, Texture->Height, Texture->PSM);
	int VramFileSize = gsKit_texture_size(Texture->Width, Texture->Height, Texture->PSM);
	Texture->Mem = memalign(128, FileSize);

	if(Texture->PSM != GS_PSM_T8 && Texture->PSM != GS_PSM_T4)
	{
		Texture->VramClut = 0;
		Texture->Clut = NULL;
	}

	if(fread(Texture->Mem, FileSize, 1, File) <= 0)
	{
		printf("Texture might be bad: %s\n", Path);
		printf("Texture size: %d\n", FileSize);
	}
	fclose(File);

	if(!Texture->Delayed)
	{
		Texture->Vram = gsKit_vram_alloc(gsGlobal, VramFileSize, GSKIT_ALLOC_USERBUFFER);
		if(Texture->Vram == GSKIT_ALLOC_ERROR)
		{
			printf("VRAM Allocation Failed. Will not upload texture.\n");
			return -1;
		}
		gsKit_texture_upload(gsGlobal, Texture);

		free(Texture->Mem);
	}
	else
	{
		gsKit_setup_tbw(Texture);
	}

	return 0;
}

int gsKit_texture_tga(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, char *Path)
{
	printf("ERROR: gsKit_texture_tga unimplimented.\n");
	return -1;
}

int gsKit_texture_fnt_raw(GSGLOBAL *gsGlobal, GSFONT *gsFont)
{
	u32 *data = (u32*)gsFont->RawData;
	u32 *mem;
	int size = 0;
	int vramsize = 0;
	int i;

	gsFont->Texture->Width  = data[1];
	gsFont->Texture->Height = data[2];
	gsFont->Texture->PSM    = data[3];
	gsFont->Texture->Filter = GS_FILTER_NEAREST;
	if(gsFont->Texture->PSM != GS_PSM_T8 && gsFont->Texture->PSM != GS_PSM_T4)
	{
		gsFont->Texture->VramClut = 0;
		gsFont->Texture->Clut = NULL;
	}

	gsFont->HChars          = data[4];
	gsFont->VChars          = data[5];
	gsFont->CharWidth       = data[6];
	gsFont->CharHeight      = data[7];

	size = gsKit_texture_size_ee(gsFont->Texture->Width, gsFont->Texture->Height, gsFont->Texture->PSM);
	vramsize = gsKit_texture_size(gsFont->Texture->Width, gsFont->Texture->Height, gsFont->Texture->PSM);
	gsFont->Texture->Mem = memalign(128, size);
	gsFont->Texture->Vram = gsKit_vram_alloc(gsGlobal, vramsize, GSKIT_ALLOC_USERBUFFER);
	if(gsFont->Texture->Vram == GSKIT_ALLOC_ERROR)
	{
		printf("VRAM Allocation Failed. Will not upload texture.\n");
		return -1;
	}

	memcpy(gsFont->Texture->Mem, &data[288/4], size);

	if (gsFont->Texture->PSM != GS_PSM_CT32) {
		printf("Unsupported fnt PSM %d\n", gsFont->Texture->PSM);
	}
	mem = (u32*)gsFont->Texture->Mem;
	for (i=0; i<size/4; i++) {
		if (mem[i] == 0xFF00FFFF) {
			mem[i] = 0;
		} else {
			u32 c = (mem[i] & 0x00FF0000) >> 16;
			mem[i] = 0x80000000 | (c) | (c<<8) | (c<<16);
		}
	}

	gsKit_texture_upload(gsGlobal, gsFont->Texture);
	free(gsFont->Texture->Mem);
	return 0;
}

// TODO:fix
/*
int gsKit_texture_fnt(GSGLOBAL *gsGlobal, GSFONT *gsFont)
{
	u32 *mem;
	int size = 0;
	int vramsize = 0;
	int i;

	FILE* File = fopen(gsFont->Path, "r");
	if (File == NULL)
	{
		printf("Failed to load font: %s\n", gsFont->Path);
		return -1;
	}
	fseek(File, 4, SEEK_SET);
	if(fread(&gsFont->Texture->Width, 4, 1, File) <= 0)
	{
		printf("Could not load font: %s\n", gsFont->Path);
		return -1;
	}
	if(fread(&gsFont->Texture->Height, 4, 1, File) <= 0)
	{
		printf("Could not load font: %s\n", gsFont->Path);
		return -1;
	}
	if(fread((int*)&gsFont->Texture->PSM, 4, 1, File) <= 0)
	{
		printf("Could not load font: %s\n", gsFont->Path);
		return -1;
	}
	if(fread(&gsFont->HChars, 4, 1, File) <= 0)
	{
		printf("Could not load font: %s\n", gsFont->Path);
		return -1;
	}
	if(fread(&gsFont->VChars, 4, 1, File) <= 0)
	{
		printf("Could not load font: %s\n", gsFont->Path);
		return -1;
	}
	if(fread(&gsFont->CharWidth, 4, 1, File) <= 0)
	{
		printf("Could not load font: %s\n", gsFont->Path);
		return -1;
	}
	if(fread(&gsFont->CharHeight, 4, 1, File) <= 0)
	{
		printf("Could not load font: %s\n", gsFont->Path);
		return -1;
	}

	fseek(File, 284, SEEK_SET);

	gsFont->Texture->Filter = GS_FILTER_NEAREST;

	size = gsKit_texture_size_ee(gsFont->Texture->Width, gsFont->Texture->Height, gsFont->Texture->PSM);
	vramsize = gsKit_texture_size(gsFont->Texture->Width, gsFont->Texture->Height, gsFont->Texture->PSM);
	gsFont->Texture->Mem = memalign(128, size); // shouldn't this just be a (char*)malloc(sizeof(char)*256);
	gsFont->Texture->Vram = gsKit_vram_alloc(gsGlobal, vramsize, GSKIT_ALLOC_USERBUFFER);

	if(gsFont->Texture->Vram == GSKIT_ALLOC_ERROR)
	{
		printf("VRAM Allocation Failed. Will not upload texture.\n");
		return -1;
	}

	if(fread(gsFont->Texture->Mem, size, 1, File) <= 0)
	{
        printf("Font might be bad: %s\n", gsFont->Path);
	}

	fclose(File);

	if (gsFont->Texture->PSM != 0) {
		printf("Unsupported fnt PSM %d\n", gsFont->Texture->PSM);
	}

	mem = (u32*)gsFont->Texture->Mem;

	for (i=0; i<size/4; i++) {
		if (mem[i] == 0xFF00FFFF) {
			mem[i] = 0;
		} else {
			u32 c = (mem[i] & 0x00FF0000) >> 16;
			mem[i] = 0x80000000 | (c) | (c<<8) | (c<<16);
		}
	}

	gsKit_texture_upload(gsGlobal, gsFont->Texture);
	free(gsFont->Texture->Mem);

	return 0;
}
*/
GSFONT *gsKit_init_font(u8 type, char *path)
{
    char *tmp = NULL;

	GSFONT *gsFont = calloc(1,sizeof(GSFONT));
	gsFont->Texture = calloc(1,sizeof(GSTEXTURE));
	gsFont->Path = calloc(1,strlen(path));
	gsFont->Additional=calloc(1,sizeof(short)*256);

    gsFont->Type = type;
    strcpy(gsFont->Path, path);

    if(gsFont->Type == GSKIT_FTYPE_BMP_DAT || gsFont->Type == GSKIT_FTYPE_PNG_DAT)
	{
        gsFont->Path_DAT = calloc(1,strlen(path));
        strcpy(gsFont->Path_DAT, path);

        if(gsFont->Type == GSKIT_FTYPE_BMP_DAT) tmp = strstr(gsFont->Path_DAT, ".bmp");
        if(gsFont->Type == GSKIT_FTYPE_PNG_DAT) tmp = strstr(gsFont->Path_DAT, ".png");

        if (tmp == NULL)
        {
            free(gsFont->Additional);
            free(gsFont->Path);
            free(gsFont->Path_DAT);
            free(gsFont->Texture);
            free(gsFont);

            printf("Error initializing .dat\n");

            return NULL;
        }
        else
        {
            strcpy(tmp, ".dat");
        }
    }
    else
    {
        gsFont->Path_DAT = NULL;
    }

    return gsFont;

}

GSFONT *gsKit_init_font_raw(u8 type, u8 *data, int size)
{

	GSFONT *gsFont = calloc(1,sizeof(GSFONT));
	gsFont->Texture = calloc(1,sizeof(GSTEXTURE));
	gsFont->RawData = data;
	gsFont->RawSize = size;
	gsFont->Type = type;
    gsFont->Additional=(short*)malloc(sizeof(short)*256);

	return gsFont;
}

int gsKit_font_upload_raw(GSGLOBAL *gsGlobal, GSFONT *gsFont)
{
	int i;

	if( gsFont->Type == GSKIT_FTYPE_FNT )
	{
		if( gsKit_texture_fnt_raw(gsGlobal, gsFont) == -1 )
		{
			printf("Error uploading font!\n");
			return -1;
		}
		for (i=0; i<256; i++) {
			gsFont->Additional[i] = (short)gsFont->CharWidth;
		}

		return 0;
	}

	return -1; //type unknown
}

int gsKit_font_upload(GSGLOBAL *gsGlobal, GSFONT *gsFont)
{
	int i;

	if( gsFont->RawData ) {
		return gsKit_font_upload_raw(gsGlobal, gsFont);
	}

	if( gsFont->Type == GSKIT_FTYPE_FNT )
	{
		//if( gsKit_texture_fnt(gsGlobal, gsFont) == -1 )
		//{
			printf("Error uploading font!\n");
			return -1;
		//}

		for (i=0; i<256; i++) {
            gsFont->Additional[i] = gsFont->CharWidth;
        }

		return 0;
	}
	else if( (gsFont->Type == GSKIT_FTYPE_PNG_DAT) ||
             (gsFont->Type == GSKIT_FTYPE_BMP_DAT))
	{
#ifdef HAVE_LIBPNG
	    if( gsFont->Type == GSKIT_FTYPE_PNG_DAT)
            if( gsKit_texture_png(gsGlobal, gsFont->Texture, gsFont->Path) == -1)
            {
                printf("Error uploading font png!\n");
                return -1;
            }
#endif
        if( gsFont->Type == GSKIT_FTYPE_BMP_DAT )
            if( gsKit_texture_bmp(gsGlobal, gsFont->Texture, gsFont->Path) == -1 )
            {
                printf("Error uploading font bmp!\n");
                return -1;
            }

		gsFont->HChars=16;
		gsFont->VChars=16;
		gsFont->CharWidth = gsFont->Texture->Width / 16;
		gsFont->CharHeight = gsFont->Texture->Height / 16;

        if(gsFont->Path_DAT != NULL) {
            FILE* File = fopen(gsFont->Path_DAT, "r");
            if (File != NULL)
            {
                fseek(File, 0, SEEK_SET);
                for(i=0; i<256; i++) {
                    if(fread(&gsFont->Additional[i], 2, 1, File) <= 0)
                    {
                        printf("Problem reading font sizes %s\n", gsFont->Path_DAT);
                        fclose(File);
                        return -1;
                    }
                }
                fclose(File);
                return 0;
            }
            else
            {
                for (i=0; i<256; i++) {
                    gsFont->Additional[i] = (short)gsFont->CharWidth;
                    return 0;
                }
            }
        }
	}

	// if it reaches here, something's wrong
	printf("Error uploading font!\n");

	return -1;
}


void gsKit_font_print_scaled(GSGLOBAL *gsGlobal, GSFONT *gsFont, float X, float Y, int Z,
                      float scale, unsigned long color, const char *String)
{
#ifdef HAVE_LIBPNG
	if( gsFont->Type == GSKIT_FTYPE_PNG_DAT)
	{
		u64 oldalpha = gsGlobal->PrimAlpha;
		u8 oldpabe = gsGlobal->PABE;
		u8 fixate = 0;
		if(gsGlobal->Test->ATE)
		{
			gsKit_set_test(gsGlobal, GS_ATEST_OFF);
			fixate = 1;
		}
		gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0,1,0,1,0), 0);


		int cx,cy,i,l;
		char c;
		cx=X;
		cy=Y;

		l=strlen( String );
		for( i=0;i<l;i++ )
		{
			c=String[i];
			if( c=='\n' )
			{
				cx=X;
				cy+=(gsFont->CharHeight*scale)+1;
			}
			else
			{
				int px,py,charsiz;

				px=c%16;
				py=(c-px)/16;
				charsiz=gsFont->Additional[(u8)c];

				gsKit_prim_sprite_texture(gsGlobal, gsFont->Texture, cx, cy,
					(px*gsFont->CharWidth), (py*gsFont->CharHeight+1),
					cx+(charsiz*scale), cy+(gsFont->CharHeight*scale),
					(px*gsFont->CharWidth)+charsiz, (py*gsFont->CharHeight)+16+1,
					Z, color);
				cx+=(charsiz*scale)+1;
			}
		}
		gsGlobal->PABE = oldpabe;
		gsGlobal->PrimAlpha=oldalpha;
		gsKit_set_primalpha(gsGlobal, gsGlobal->PrimAlpha, gsGlobal->PABE);
		//gsKit_set_clamp(gsGlobal, GS_CMODE_RESET);

		if(fixate)
			gsKit_set_test(gsGlobal, GS_ATEST_ON);

	}
#endif
	if( gsFont->Type == GSKIT_FTYPE_BMP_DAT ||
		gsFont->Type == GSKIT_FTYPE_FNT)
	{
		u64 oldalpha = gsGlobal->PrimAlpha;
		gsGlobal->PrimAlpha=ALPHA_BLEND_ADD;

		int cx,cy,i,l;
		char c;
		cx=X;
		cy=Y;

		l=strlen( String );
		for( i=0;i<l;i++ )
		{
			c=String[i];
			if( c=='\n' )
			{
				cx=X;
				cy+=(gsFont->CharHeight*scale)+1;
			}
			else
			{
				int px,py,charsiz;

				px=c%16;
				py=(c-px)/16;
				charsiz=gsFont->Additional[(u8)c];

				gsKit_prim_sprite_texture(gsGlobal, gsFont->Texture, cx, cy,
					(px*gsFont->CharWidth), (py*gsFont->CharHeight+1),
					cx+(charsiz*scale), cy+(gsFont->CharHeight*scale),
					(px*gsFont->CharWidth)+charsiz, (py*gsFont->CharHeight)+16+1,
					Z, color);
				cx+=(charsiz*scale)+1;
			}
		}

		gsGlobal->PrimAlpha=oldalpha;
	}
}
