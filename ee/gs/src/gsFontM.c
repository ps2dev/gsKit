//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsFontM.c - Font loading, manipulation, handling, and printing.
//
// FONTM Unpacking Code (_unpack_raw and _unpack_raw_1 routines) borrowed from
// [RO]man's unpack.c @ http://ps2dev.pgamers.com/
//
// Other FONTM help and info from adresd.
//
// Greets and thanks to both of you!
//

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>

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
static u32 gsKit_fontm_clut[16] __attribute__((aligned(128))) = {
	0x00000000,
	0x11111111,
	0x22222222,
	0x33333333,
	0x44444444,
	0x55555555,
	0x66666666,
	0x77777777,
	0x80888888,
	0x80999999,
	0x80AAAAAA,
	0x80BBBBBB,
	0x80CCCCCC,
	0x80DDDDDD,
	0x80EEEEEE,
	0x80FFFFFF
};


GSFONTM *gsKit_init_fontm(void)
{
	int pgindx;
	GSFONTM *gsFontM;

	gsFontM = calloc(1, sizeof(GSFONTM));
	for (pgindx=0; pgindx<GS_FONTM_PAGE_COUNT; pgindx++)
		gsFontM->Texture[pgindx] = calloc(1, sizeof(GSTEXTURE));

	return gsFontM;
}

void gsKit_free_fontm(GSGLOBAL *gsGlobal, GSFONTM *gsFontM)
{
	int pgindx;

	for (pgindx=0; pgindx<GS_FONTM_PAGE_COUNT; pgindx++) {
		// Free from vram
		gsKit_TexManager_free(gsGlobal, gsFontM->Texture[pgindx]);
		// clut was pointing to static memory, so do not free
		gsFontM->Texture[pgindx]->Clut = NULL;
		// mem was pointing to 'TexBase', so do not free
		gsFontM->Texture[pgindx]->Mem = NULL;
		// free texture
		free(gsFontM->Texture[pgindx]);
		gsFontM->Texture[pgindx] = NULL;
	}

	if (gsFontM->Header.offset_table != NULL)
		free(gsFontM->Header.offset_table);

	if (gsFontM->TexBase != NULL)
		free(gsFontM->TexBase);

	free(gsFontM);
}

int gsKit_fontm_upload(GSGLOBAL *gsGlobal, GSFONTM *gsFontM)
{
    int pgindx;

    if(gsKit_fontm_unpack(gsFontM) == -1)
		return -1;

	for (pgindx=0; pgindx<GS_FONTM_PAGE_COUNT; pgindx++) {
		gsFontM->Texture[pgindx]->Width = 52;
	    gsFontM->Texture[pgindx]->Height = 832;
	    gsFontM->Texture[pgindx]->PSM = GS_PSM_T4;
	    gsFontM->Texture[pgindx]->ClutPSM = GS_PSM_CT32;
	    gsFontM->Texture[pgindx]->Filter = GS_FILTER_LINEAR;
		gsFontM->Texture[pgindx]->Clut = gsKit_fontm_clut;
	    gsKit_setup_tbw(gsFontM->Texture[pgindx]);
	}

	gsFontM->VramIdx = 0;
	gsFontM->Spacing = 1.0f;
	gsFontM->Align = GSKIT_FALIGN_LEFT;

	return 0;
}

int gsKit_fontm_unpack(GSFONTM *gsFontM)
{
	void *packed;
	void *unpacked;

	int FontMFile = open("rom0:FONTM", O_RDONLY);
	if(FontMFile < 0)
	{
		printf("Failed to open FONTM from ROM0\n");
		return -1;
	}

	int PackedSize = lseek(FontMFile, 0, SEEK_END);

	lseek(FontMFile, 0, SEEK_SET);

	packed = malloc(PackedSize);

	if(read(FontMFile, packed, PackedSize) != PackedSize)
	{
		printf("Error Reading Packed FONTM Data\n");
		close(FontMFile);
		return -1;
	}

	close(FontMFile);

	struct gsKit_fontm_unpack updata;

	updata.size = *(u32 *)packed;
	updata.ptr = packed + 4;

	unpacked = malloc(updata.size);

	gsKit_fontm_unpack_raw(unpacked, &updata);

	free(packed);

/*
	// Use this to dump the unpacked fontm file to host:
	int DumpFile = open("host:fontm.dump", O_RDWR);

	write(DumpFile, unpacked, updata.size);

	close(DumpFile);
*/

	gsFontM->Header.sig = *(u32 *)unpacked;
	gsFontM->Header.version = *(u32 *)(unpacked + 4);
	gsFontM->Header.bitsize = *(u32 *)(unpacked + 8);
	gsFontM->Header.baseoffset = *(u32 *)(unpacked + 12);
	gsFontM->Header.num_entries = *(u32 *)(unpacked + 16);
	gsFontM->Header.eof = *(u32 *)(unpacked + 20);
	int TableSize = (gsFontM->Header.num_entries * 4);
	gsFontM->Header.offset_table = malloc(TableSize);

	u8 *temp;
	temp = (u8 *)((u32)unpacked + gsFontM->Header.baseoffset + 17680);

	int TexSize = (338 * gsFontM->Header.num_entries);

	gsFontM->TexBase = memalign(128, TexSize);

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

	u32 linemax = (gsFontM->Header.num_entries * 26) - 52;

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
	for(offset = 0; offset < gsFontM->Header.num_entries; offset++)
	{
		mask32 = (offset << 31);
		if(mask32)
			gsFontM->Header.offset_table[offset] = gsFontM->Header.offset_table[offset - 1];
		else
			gsFontM->Header.offset_table[offset] = (offset * 338);
	}

	// Swap nibbles around so it shows up correctly
	int byte;
	u8 *tmpdst = (u8 *)((u32)gsFontM->TexBase | 0x30000000);
	u8 *tmpsrc = (u8 *)temp2;

	for(byte = 0; byte < TexSize; byte++)
	{
		tmpdst[byte] = (tmpsrc[byte] << 4) | (tmpsrc[byte] >> 4);
	}

	free(temp2);

	free(unpacked);

#ifdef  GSKIT_DEBUG
	printf("FONTM Header:\n");
	printf("\tSIG: 0x%08X\n", gsFontM->Header.sig);
	printf("\tVersion: 0x%08X\n", gsFontM->Header.version);
	printf("\tBitsize: 0x%08X\n", gsFontM->Header.bitsize);
	printf("\tBaseOffset: 0x%08X\n", gsFontM->Header.baseoffset);
	printf("\tNum Entries: 0x%08X\n", gsFontM->Header.num_entries);
	printf("\tEOF: 0x%08X\n", gsFontM->Header.eof);
#endif

	return 0;
}

void gsKit_fontm_unpack_raw(u8 *base, struct gsKit_fontm_unpack *oke)
{
	u32 val, count, i;
	u8 *p, *src;

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

void gsKit_fontm_print_scaled(GSGLOBAL *gsGlobal, GSFONTM *gsFontM, float X, float Y, int Z,
                      float scale, unsigned long color, const char *String)
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
		else if(cur == '\t' && gsFontM->Align != GSKIT_FALIGN_LEFT)
		{
			printf("WARNING: Tabs not supported in GSKIT_FALIGN_CENTER/RIGHT\n");
			break;
		}
	}
	numlines++;

	for(curline = 0; curline < numlines; curline++)
	{
		if(gsFontM->Align == GSKIT_FALIGN_LEFT)
			posx[curline] = X;
		else if(gsFontM->Align == GSKIT_FALIGN_CENTER)
			posx[curline] = X - (((26.0f * linechars[curline]) * gsFontM->Spacing * scale) / 2.0f);
		else if(gsFontM->Align == GSKIT_FALIGN_RIGHT)
			posx[curline] = X - ((26.0f * linechars[curline]) * gsFontM->Spacing * scale);
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
			posx[curline] += (26.0f * gsFontM->Spacing * scale);
		}
		else
		{
			int idx;
			int pgindx;
			uoffset = 0;

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
				for(slen = 0; ((pos + slen) < length) &&
									 String[pos + slen] >= 'a' &&
									 String[pos + slen] <= 'z'; slen++);

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

			aligned = ((u32)gsFontM->TexBase + gsFontM->Header.offset_table[idxoffset * 64]);

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

			for (pgindx=0; pgindx<GS_FONTM_PAGE_COUNT; pgindx++) {
				if (gsFontM->Texture[pgindx]->Mem == (void *)aligned) {
					// Page found
					break;
				}
			}
			if (pgindx >= GS_FONTM_PAGE_COUNT) {
				// Page not found
				// Try to find unused page first
				for (pgindx=0; pgindx<GS_FONTM_PAGE_COUNT; pgindx++) {
					if (gsFontM->Texture[pgindx]->Mem == 0) {
						// Unused page found
						break;
					}
				}
				// Invalidate a page if no free page found
				if (pgindx >= GS_FONTM_PAGE_COUNT) {
					pgindx = (gsFontM->VramIdx + 1) % GS_FONTM_PAGE_COUNT;
					gsKit_TexManager_invalidate(gsGlobal, gsFontM->Texture[pgindx]);
				}

				gsFontM->Texture[pgindx]->Mem = (void *)aligned;
				//printf("Mem[%d] = 0x%x\n", pgindx, aligned);
			}

			gsFontM->VramIdx = pgindx; // Last used page index
			gsKit_TexManager_bind(gsGlobal, gsFontM->Texture[pgindx]);
            gsKit_prim_sprite_texture(gsGlobal, gsFontM->Texture[pgindx], posx[curline], posy, uoffset, voffset,
                (float)posx[curline] + (26.0f * scale), (float)posy + (26.0f * scale), uoffset + 26, voffset + 27, Z, color);

            posx[curline] += (26.0f * gsFontM->Spacing * scale);
        }
    }

    gsGlobal->PABE = oldpabe;
    gsGlobal->PrimAlpha=oldalpha;
    gsKit_set_primalpha(gsGlobal, gsGlobal->PrimAlpha, gsGlobal->PABE);
    gsKit_set_clamp(gsGlobal, GS_CMODE_RESET);

    if(fixate)
        gsKit_set_test(gsGlobal, GS_ATEST_ON);
}
