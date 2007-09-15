//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsFont.c - Font loading, manipulation, handling, and printing.
//
// FONTM Unpacking Code (_unpack_raw and _unpack_raw_1 routines) borrowed from
// [RO]man's unpack.c @ http://ps2dev.pgamers.com/
//
// Other FONTM help and info from adresd.
//
// Greets and thanks to both of you!
//

#include <stdio.h>
#include <string.h>

#include "gsKit.h"
#include "gsInline.h"

/// ASCII <-> FONTM Mapping Table
/// The index passed to the array is the ASCII character.
/// It returns the fontm character number which relates to the ASCII one.
static u32 gsKit_fontm_ascii[128] = {	   0,   0,   0,   0,   0,   0,   0,   0,  // 0 - 7
					   0,   0,   0,   0,   0,   0,   0,   0,  // 8 - 15
					   0,   0,   0,   0,   0,   0,   0,   0,  // 16 - 23
					   0,   0,   0,   0,   0,   0,   0,   0,  // 24 - 31
					   0,   9,  40,  83,  79,  82,  84,  38,  // 32 - 39
					  41,  42,  85,  59,   3,  60,   4,  30,  // 40 - 47
					 203, 204, 205, 206, 207, 208, 209, 210,  // 48 - 55
					 211, 212,   6,   7,  66,  64,  67,   8,  // 56 - 63
					  86, 220, 221, 222, 223, 224, 225, 226,  // 64 - 71
					 227, 228, 229, 230, 231, 232, 233, 234,  // 72 - 79
					 235, 236, 237, 238, 239, 240, 241, 242,  // 80 - 87
					 243, 244, 245,  45,  31,  46,  15,  17,  // 88 - 95
					  13, 252, 253, 254, 255, 256, 257, 258,  // 96 - 103
					 259, 260, 261, 262, 263, 264, 265, 266,  // 104 - 111
					 267, 268, 269, 270, 271, 272, 273, 274,  // 112 - 119
					 275, 276, 277,  47,  34,  48,  32,   0 }; // 120 - 127

/// FONTM CLUT
/// FONTM Textures are GS_PSM_T4, and need a 16x16 CLUT
/// This is a greyscale ramp CLUT, with linear alpha.
static u32 gsKit_fontm_clut[16] = {	0x00000000, 0x11111111, 0x22222222, 0x33333333, \
					0x44444444, 0x55555555, 0x66666666, 0x77777777, \
					0x80888888, 0x80999999, 0x80AAAAAA, 0x80BBBBBB, \
					0x80CCCCCC, 0x80DDDDDD, 0x80EEEEEE, 0x80FFFFFF };

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
		gsFont->Additional=malloc( 0x100 );
		for (i=0; i<0x100; i++) {
			gsFont->Additional[i] = gsFont->CharWidth;
		}

		return 0;		
	}

	return -1; //type unknown
}

int gsKit_font_upload(GSGLOBAL *gsGlobal, GSFONT *gsFont)
{
    int pgindx;
	int i;

	if( gsFont->RawData ) {
		return gsKit_font_upload_raw(gsGlobal, gsFont);
	}

	if( gsFont->Type == GSKIT_FTYPE_FNT )
	{
		if( gsKit_texture_fnt(gsGlobal, gsFont) == -1 )
		{
			printf("Error uploading font!\n");
			return -1; 
		}
		gsFont->Additional=malloc( 0x100 );
		for (i=0; i<0x100; i++) {
			gsFont->Additional[i] = gsFont->CharWidth;
		}

		return 0;
	}
#ifdef HAVE_LIBPNG
	else if( gsFont->Type == GSKIT_FTYPE_PNG_DAT )
	{
		if( gsKit_texture_png(gsGlobal, gsFont->Texture, gsFont->Path_PNG) == -1)
		{
			printf("Error uploading font png!\n");
			return -1;
		}

		gsFont->HChars=16;
		gsFont->VChars=16;
		gsFont->CharWidth = gsFont->Texture->Width / 16;
		gsFont->CharHeight = gsFont->Texture->Height / 16;

		int File = fioOpen(gsFont->Path_DAT, O_RDONLY);
		gsFont->Additional=malloc( 0x100 );
		u16 temp_buffer[512];
		if (File > 0)
		{
			fioLseek(File, 0, SEEK_SET);
			if(fioRead(File, &temp_buffer, 0x200) <= 0)
			{
				printf("Could not load font sizes: %s\n", gsFont->Path_DAT);
				return -1;
			}
			fioClose(File);
			for (i = 0; i < 0x100; i++)
			{
				gsFont->Additional[i] = temp_buffer[i];
			}
		}
		else
		{
			int i;
			for (i = 0; i < 0x100; i++)
			{
				gsFont->Additional[i] = gsFont->CharWidth;
			}
		}

		return 0;
	}
#endif
	else if( gsFont->Type == GSKIT_FTYPE_BMP_DAT )
	{
		if( gsKit_texture_bmp(gsGlobal, gsFont->Texture, gsFont->Path_BMP) == -1 )
		{
			printf("Error uploading font bmp!\n");
			return -1;
		}

		gsFont->HChars=16;
		gsFont->VChars=16;
		gsFont->CharWidth = gsFont->Texture->Width / 16;
		gsFont->CharHeight = gsFont->Texture->Height / 16;

		int File = fioOpen(gsFont->Path_DAT, O_RDONLY);
		gsFont->Additional=malloc( 0x100 );
		if (File > 0)
		{
			fioLseek(File, 0, SEEK_SET);
			if(fioRead(File, gsFont->Additional, 0x100) <= 0)
			{
				printf("Could not load font sizes: %s\n", gsFont->Path_DAT);
				return -1;
			}
			fioClose(File);
		}
		else
		{
			for (i = 0; i < 0x100; i++)
			{
				gsFont->Additional[i] = gsFont->CharWidth;
			}
		}

		return 0;
	}
	else if( gsFont->Type == GSKIT_FTYPE_FONTM )
	{
		if(gsKit_fontm_unpack(gsFont) == -1)
			return -1;

		gsFont->Texture->Width = 52;
		gsFont->Texture->Height = 832;
		gsFont->Texture->PSM = GS_PSM_T4;
		gsFont->Texture->ClutPSM = GS_PSM_CT32;
		gsFont->Texture->Filter = GS_FILTER_LINEAR;
		
		gsFont->Texture->TBW = (-GS_VRAM_TBWALIGN_CLUT)&(gsFont->Texture->Width+GS_VRAM_TBWALIGN_CLUT-1);
		if(gsFont->Texture->TBW / 64 > 0)
			gsFont->Texture->TBW = (gsFont->Texture->TBW / 64);
		else
			gsFont->Texture->TBW = 1;

		gsFont->Texture->VramClut = gsKit_vram_alloc(gsGlobal, 4096, GSKIT_ALLOC_USERBUFFER);
		int TexSize = gsKit_texture_size(gsFont->Texture->Width, gsFont->Texture->Height, gsFont->Texture->PSM);
        for(pgindx = 0; pgindx < GS_FONT_PAGE_COUNT; ++pgindx)
        {
            gsFont->FontM_Vram[pgindx] = gsKit_vram_alloc(gsGlobal, TexSize, GSKIT_ALLOC_USERBUFFER);
            gsFont->FontM_LastPage[pgindx] = (u32) -1;
        }
		gsFont->Texture->Vram = gsFont->FontM_Vram[0];
		gsFont->FontM_VramIdx = 0;
		gsFont->FontM_Spacing = 1.0f;
		gsFont->FontM_Align = GSKIT_FALIGN_LEFT;

		gsFont->Texture->Clut = memalign(128, 64);
		memcpy(gsFont->Texture->Clut, gsKit_fontm_clut, 64);
		gsKit_texture_send(gsFont->Texture->Clut, 8,  2, gsFont->Texture->VramClut, gsFont->Texture->ClutPSM, 1, GS_CLUT_PALLETE);
		free(gsFont->Texture->Clut);

		return 0;
	}
	else 
		return -1; //type unknown
}

int gsKit_fontm_unpack(GSFONT *gsFont)
{
	void *packed;
	void *unpacked;

	int FontMFile = fioOpen("rom0:FONTM", O_RDONLY);
	if(FontMFile < 0)
	{
		printf("Failed to open FONTM from ROM0\n");
		return -1;
	}

	int PackedSize = fioLseek(FontMFile, 0, SEEK_END);
	
	fioLseek(FontMFile, 0, SEEK_SET);

	packed = malloc(PackedSize);

	if(fioRead(FontMFile, packed, PackedSize) != PackedSize)
	{
		printf("Error Reading Packed FONTM Data\n");
		fioClose(FontMFile);
		return -1;
	}

	fioClose(FontMFile);

	struct gsKit_fontm_unpack updata;

	updata.size = *(u32 *)packed;
	updata.ptr = packed + 4;

	unpacked = malloc(updata.size);

	gsKit_fontm_unpack_raw(unpacked, &updata);

	free(packed);

/*
	// Use this to dump the unpacked fontm file to host:
	int DumpFile = fioOpen("host:fontm.dump", O_RDWR);

	fioWrite(DumpFile, unpacked, updata.size);

	fioClose(DumpFile);
*/

	gsFont->FontM_Header.sig = *(u32 *)unpacked;
	gsFont->FontM_Header.version = *(u32 *)(unpacked + 4);
	gsFont->FontM_Header.bitsize = *(u32 *)(unpacked + 8);
	gsFont->FontM_Header.baseoffset = *(u32 *)(unpacked + 12);
	gsFont->FontM_Header.num_entries = *(u32 *)(unpacked + 16);
	gsFont->FontM_Header.eof = *(u32 *)(unpacked + 20);
	int TableSize = (gsFont->FontM_Header.num_entries * 4);
	gsFont->FontM_Header.offset_table = malloc(TableSize);

	u8 *temp;
	(u32)temp = ((u32)unpacked + gsFont->FontM_Header.baseoffset + 17680);

	int TexSize = (338 * gsFont->FontM_Header.num_entries);

	gsFont->TexBase = memalign(128, TexSize);

	u8 *temp2 = malloc(TexSize);

	u8 numlines, lineno, mask;

	u32 offset;

	struct line
	{
		u64	blah1;
		u32	blah2;
		u8	blah3;
	} __attribute__ ((packed));

	typedef struct line PADLINE;

	PADLINE *olddata = (PADLINE *)temp;
	PADLINE *newdata = (PADLINE *)temp2;

	u32 linemax = (gsFont->FontM_Header.num_entries * 26) - 52;

	// Interleave lines so we can deal with it as two columns of glyphs in vram
	for(offset = 0; offset < linemax; offset += 52)
	{
		lineno = 0;
		for(numlines = 0; numlines < 52; numlines++)
		{
			mask = (numlines << 7);
			if(mask)
			{
				newdata[numlines + offset] = olddata[offset + lineno + 26];
				lineno++;
			}
			else
			{
				newdata[numlines + offset] = olddata[offset + lineno];
			}
		}
	}

	u32 mask32;

	// Create our new offset table
	for(offset = 0; offset < gsFont->FontM_Header.num_entries; offset++)
	{
		mask32 = (offset << 31);
		if(mask32)
			gsFont->FontM_Header.offset_table[offset] = gsFont->FontM_Header.offset_table[offset - 1];
		else
			gsFont->FontM_Header.offset_table[offset] = (offset * 338);
	}

	// Swap nibbles around so it shows up correctly
	int byte;
	u8 *tmpdst = (u8 *)((u32)gsFont->TexBase | 0x30000000);
	u8 *tmpsrc = (u8 *)temp2;

	for(byte = 0; byte < TexSize; byte++)
	{
		tmpdst[byte] = (tmpsrc[byte] << 4) | (tmpsrc[byte] >> 4);
	}

	free(temp2);

	free(unpacked);

#ifdef  GSKIT_DEBUG
	printf("FONTM Header:\n");
	printf("\tSIG: 0x%08X\n", gsFont->FontM_Header.sig);
	printf("\tVersion: 0x%08X\n", gsFont->FontM_Header.version);
	printf("\tBitsize: 0x%08X\n", gsFont->FontM_Header.bitsize);
	printf("\tBaseOffset: 0x%08X\n", gsFont->FontM_Header.baseoffset);
	printf("\tNum Entries: 0x%08X\n", gsFont->FontM_Header.num_entries);
	printf("\tEOF: 0x%08X\n", gsFont->FontM_Header.eof);
#endif

	return 0;
}

void gsKit_fontm_unpack_raw(unsigned char *base, struct gsKit_fontm_unpack *oke)
{
	unsigned int val, count, i;
	unsigned char *p, *src;

	for (i=0, p=base; p < base+oke->size; i--, oke->data <<= 1)
	{
		if (i == 0)
		{
			i = 30;
			gsKit_fontm_unpack_raw_1(oke);
		}

		if (oke->data & 0x80000000)
		{
			val  = *oke->ptr++ << 8;
			val |= *oke->ptr++;
			src = p - (val & oke->shft) - 1;
			count = (val >> oke->dif) + 2;

			
			do
			{
				*p++ = *src++;
			}while(count--);
		}
		else
			*p++ = *oke->ptr++;
	}
}

void gsKit_fontm_unpack_raw_1(struct gsKit_fontm_unpack *oke)
{
	oke->data =             *oke->ptr++  << 8;
	oke->data = (oke->data | *oke->ptr++) << 8;
	oke->data = (oke->data | *oke->ptr++) << 8;
	oke->data =  oke->data | *oke->ptr++;
	oke->shft=0x3FFF >> (oke->data & 3);
	oke->dif=14 - (oke->data & 3);
	oke->ande=oke->data & 3;
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
				cy+=gsFont->CharHeight;
			}
			else
			{
				int px,py,charsiz;
				px=c%16;
				py=(c-px)/16;
				charsiz=gsFont->Additional[(u8)c]*1.5;

				gsKit_prim_sprite_texture(gsGlobal, gsFont->Texture, cx, cy,
					px*gsFont->CharWidth+1, py*gsFont->CharHeight+1,
					cx+gsFont->CharWidth, cy+gsFont->CharHeight,
					(px+1)*gsFont->CharWidth, (py+1)*gsFont->CharHeight,
					Z, color);

				cx+=charsiz+1;
			}
		}
		gsGlobal->PABE = oldpabe;
		gsGlobal->PrimAlpha=oldalpha;
		gsKit_set_primalpha(gsGlobal, gsGlobal->PrimAlpha, gsGlobal->PABE);
		//gsKit_set_clamp(gsGlobal, GS_CMODE_RESET);

		if(fixate)
			gsKit_set_test(gsGlobal, GS_ATEST_ON);

	}
	else if( gsFont->Type == GSKIT_FTYPE_BMP_DAT ||
		gsFont->Type == GSKIT_FTYPE_FNT)
	{
#else
	if( gsFont->Type == GSKIT_FTYPE_BMP_DAT ||
		gsFont->Type == GSKIT_FTYPE_FNT)
	{
#endif
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
				cy+=gsFont->CharHeight;
			}
			else
			{
				int px,py,charsiz;
				px=c%16;
				py=(c-px)/16;
				charsiz=gsFont->Additional[(u8)c];

				gsKit_prim_sprite_texture(gsGlobal, gsFont->Texture, cx, cy, 
					px*gsFont->CharWidth+1, py*gsFont->CharHeight+1, 
					cx+gsFont->CharWidth, cy+gsFont->CharHeight, 
					(px+1)*gsFont->CharWidth, (py+1)*gsFont->CharHeight, 
					Z, color);

				cx+=charsiz+1;
			}
		}

		gsGlobal->PrimAlpha=oldalpha;
	}
	else if(gsFont->Type == GSKIT_FTYPE_FONTM)
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

		int length = strlen(String);
		int pos;

		u32 linechars[GSKIT_FONTM_MAXLINES];

		float posx[GSKIT_FONTM_MAXLINES], posy = Y;
 
		char cur;

		u32 aligned, idxoffset, idxremain;
		u32 voffset = 0;
		u32 uoffset = 0;
		u8 numlines = 0;
		u8 curline = 0;
		linechars[0] = 0;

		int tabscale = (104.0f * scale);

		for(pos = 0; pos < length; pos++)
		{
			cur = String[pos];
			if(cur != '\n' && cur != '\t')
			{
				linechars[numlines]++;
			}
			else if(cur == '\n')
			{
				numlines++;
				linechars[numlines] = 0;
			}
			else if(cur == '\t' && gsFont->FontM_Align != GSKIT_FALIGN_LEFT)
			{
				printf("WARNING: Tabs not supported in GSKIT_FALIGN_CENTER/RIGHT\n");
				break;
			}
		}
		numlines++;

		for(curline = 0; curline < numlines; curline++)
		{
			if(gsFont->FontM_Align == GSKIT_FALIGN_LEFT)
				posx[curline] = X;
			else if(gsFont->FontM_Align == GSKIT_FALIGN_CENTER)
				posx[curline] = X - (((26.0f * linechars[curline]) * gsFont->FontM_Spacing * scale) / 2.0f);
			else if(gsFont->FontM_Align == GSKIT_FALIGN_RIGHT)
				posx[curline] = X - ((26.0f * linechars[curline]) * gsFont->FontM_Spacing * scale);
		}

		curline = 0;

		for(pos = 0; pos < length; pos++)
		{
			cur = String[pos];

			if(cur == '\n')
			{
				curline++;
				posy += (26.0f * scale);
			}
			else if(cur == '\t')
			{
				
				u8 tab = (posx[curline] / tabscale);
				if((int)posx % tabscale)
					tab++;
				posx[curline] = (tab * tabscale);
			}
			else if(cur == ' ')
			{
				posx[curline] += (26.0f * gsFont->FontM_Spacing * scale);
			}
			else
			{
				int idx;
                int pgindx;
				uoffset = 0;
                gsFont->pgcount = 0;

				if(cur == '\f')
				{
					char code[4];
					memcpy(code, &String[pos+1], 4);
					pos+=4;
					idx = atoi(code);
				}
				else if(cur == '\e')
				{
					u32 slen = 0;
					pos++;
					for(slen = 0; ((pos + slen) < length) && String[pos + slen] != ' ' &&
										 String[pos + slen] != '\0' &&
										 String[pos + slen] != '\t' &&
										 String[pos + slen] != '\n'; slen++);
					
					if((pos+slen) <= length)
					{
						char extstr[slen + 1];
						memcpy(extstr, &String[pos], slen);
						extstr[slen] = 0x00;
	
						pos += slen;
	
						if(!strcmp(extstr, "le"))
							idx = 68;
						else if(!strcmp(extstr, "ge"))
							idx = 69;
						else if(!strcmp(extstr, "inf"))
							idx = 70;
						else if(!strcmp(extstr, "male"))
							idx = 72;
						else if(!strcmp(extstr, "female"))
							idx = 73;
						else if(!strcmp(extstr, "degc"))
							idx = 77;
						else if(!strcmp(extstr, "yen"))
							idx = 78;
						else if(!strcmp(extstr, "cent"))
							idx = 80;
						else if(!strcmp(extstr, "pound"))
							idx = 81;
						else if(!strcmp(extstr, "left"))
							idx = 793;
						else if(!strcmp(extstr, "right"))
							idx = 792;
						else if(!strcmp(extstr, "fup"))
							idx = 798;
						else if(!strcmp(extstr, "fdown"))
							idx = 799;
						else if(!strcmp(extstr, "fleft"))
							idx = 797;
						else if(!strcmp(extstr, "fright"))
							idx = 796;
						else if(!strcmp(extstr, "hup"))
							idx = 802;
						else if(!strcmp(extstr, "hdown"))
							idx = 803;
						else if(!strcmp(extstr, "hleft"))
							idx = 801;
						else if(!strcmp(extstr, "hright"))
							idx = 800;
						else if(!strcmp(extstr, "ms"))
							idx = 864;
						else if(!strcmp(extstr, "us"))
							idx = 865;
						else if(!strcmp(extstr, "ns"))
							idx = 866;
						else if(!strcmp(extstr, "degf"))
							idx = 868;
						else if(!strcmp(extstr, "mbit"))
							idx = 869;
						else if(!strcmp(extstr, "hz"))
							idx = 871;
						else if(!strcmp(extstr, "kb"))
							idx = 873;
						else if(!strcmp(extstr, "mb"))
							idx = 874;
						else if(!strcmp(extstr, "gb"))
							idx = 875;
						else if(!strcmp(extstr, "tb"))
							idx = 876;
						else
							idx = 0;
					}
					else
						idx = 0;

					pos--;
				}
				else
					idx = gsKit_fontm_ascii[(int)cur];

				idxoffset = idx / 64;
				idxremain = idx % 64;

				aligned = ((u32)gsFont->TexBase + gsFont->FontM_Header.offset_table[idxoffset * 64]);

				if(idxremain % 2)
					uoffset = 26;

				idxremain /= 2;

				voffset = (idxremain * 26);

				u64 *p_data = gsKit_heap_alloc(gsGlobal, 1, 16, GIF_AD);

				*p_data++ = GIF_TAG_AD(1);
				*p_data++ = GIF_AD;

				*p_data++ = GS_SETREG_CLAMP(GS_CMODE_REGION_CLAMP, GS_CMODE_REGION_CLAMP,
							uoffset, uoffset + 25,
							voffset, voffset + 25);

				*p_data++ = GS_CLAMP_1+gsGlobal->PrimContext;

                for(pgindx = 0; pgindx < GS_FONT_PAGE_COUNT; ++pgindx)
                {
                    if (gsFont->FontM_LastPage[pgindx] == aligned)
                    {
                        gsFont->Texture->Vram = gsFont->FontM_Vram[pgindx];
                        gsFont->FontM_VramIdx = (pgindx + 1) % GS_FONT_PAGE_COUNT;
                        break;
                    }
                }
				if(pgindx >= GS_FONT_PAGE_COUNT)
				{
                    //printf("FontM Upload %d %d\n", gsFont->FontM_VramIdx, aligned);
                    ++gsFont->pgcount;
					gsFont->Texture->Vram = gsFont->FontM_Vram[gsFont->FontM_VramIdx];
					gsKit_texture_send_inline(gsGlobal, (void *)aligned, gsFont->Texture->Width, gsFont->Texture->Height, gsFont->Texture->Vram, GS_PSM_T4, gsFont->Texture->TBW, GS_CLUT_NONE);
					//gsKit_texture_send_inline(gsGlobal, (void *)aligned, gsFont->Texture->Width, gsFont->Texture->Height, gsFont->Texture->Vram, GS_PSM_T4, gsFont->Texture->TBW, GS_CLUT_NONE);
					gsFont->FontM_LastPage[gsFont->FontM_VramIdx] = aligned;
					gsFont->FontM_VramIdx = (++gsFont->FontM_VramIdx) % GS_FONT_PAGE_COUNT;
				}

				gsKit_prim_sprite_texture(gsGlobal, gsFont->Texture, posx[curline], posy, uoffset, voffset,
					(float)posx[curline] + (26.0f * scale), (float)posy + (26.0f * scale), uoffset + 26, voffset + 27, Z, color);

				posx[curline] += (26.0f * gsFont->FontM_Spacing * scale);
			}
		}

		gsGlobal->PABE = oldpabe;
		gsGlobal->PrimAlpha=oldalpha;
		gsKit_set_primalpha(gsGlobal, gsGlobal->PrimAlpha, gsGlobal->PABE);
		gsKit_set_clamp(gsGlobal, GS_CMODE_RESET);

		if(fixate)
			gsKit_set_test(gsGlobal, GS_ATEST_ON);
	}
	else
	{
		//font type unknown
	}
}
