//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2017 - Rick "Maximus32" Gaiser <rgaiser@gmail.com>
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsHires.c - Multi pass high resolution rendering
//

#include <stdio.h>
#include <malloc.h>
#include <kernel.h>
#include "gsTexManager.h"


struct SVramBlock {
	unsigned int iStart;
	unsigned int iSize;
	unsigned int iUseCount;
	unsigned int iUseCountPrev;

	GSTEXTURE * tex;
	struct SVramBlock * pNext;
	struct SVramBlock * pPrev;
};
static struct SVramBlock * head = NULL;
static enum ETransferMode trmode = ETM_INLINE;


//---------------------------------------------------------------------------
// Private functions

//---------------------------------------------------------------------------
static struct SVramBlock *
_blockCreate(unsigned int start, unsigned int size)
{
	struct SVramBlock * block;

	block = malloc(sizeof(struct SVramBlock));

	block->iStart = start;
	block->iSize = size;
	block->iUseCount = 0;
	block->iUseCountPrev = 0;

	block->tex = NULL;
	block->pNext = NULL;
	block->pPrev = NULL;

	return block;
}

//---------------------------------------------------------------------------
static void
_blockInsertAfter(struct SVramBlock * block, struct SVramBlock * next)
{
	next->pNext = block->pNext;
	next->pPrev = block;
	if(block->pNext != NULL)
		block->pNext->pPrev = next;
	block->pNext = next;
}

//---------------------------------------------------------------------------
static struct SVramBlock *
_blockRemove(struct SVramBlock * block)
{
	struct SVramBlock * next = block->pNext;

	if(block->pPrev != NULL)
		block->pPrev->pNext = block->pNext;
	if(block->pNext != NULL)
		block->pNext->pPrev = block->pPrev;

	free(block);

	return next;
}

//---------------------------------------------------------------------------
static struct SVramBlock *
_blockSplitFree(struct SVramBlock * block, unsigned int size)
{
	struct SVramBlock * pNewBlock;

	// Create second block with leftover size
	pNewBlock = _blockCreate(block->iStart + size, block->iSize - size);
	// Shrink first block
	block->iSize = size;
	// Insert second block after first block
	_blockInsertAfter(block, pNewBlock);

	return block;
}

//---------------------------------------------------------------------------
static struct SVramBlock *
_blockMergeFree(struct SVramBlock * block)
{
	// Search backwards to the first free block
	while ((block->pPrev != NULL) && (block->pPrev->tex == NULL))
		block = block->pPrev;

	// Merge free blocks
	while((block->pNext != NULL) && (block->pNext->tex == NULL)) {
		block->iSize += block->pNext->iSize;
		_blockRemove(block->pNext);
	}

	return block;
}

//---------------------------------------------------------------------------
// How often is this vram block used, mainly based on predictions
unsigned int
_blockGetWeight(struct SVramBlock * block)
{
	unsigned int weight = 0;

	if ((block != NULL) && (block->tex != NULL)) {
		if(block->iUseCount == block->iUseCountPrev) {
			// Prediction:
			// - This frame: done
			// - Next frame: needed
			weight += block->iUseCountPrev;
		}
		else if(block->iUseCount < block->iUseCountPrev) {
			// Prediction:
			// - This frame: needed
			weight += block->iUseCountPrev - block->iUseCount;
			// - Next frame: needed
			weight += block->iUseCountPrev;
		}
		else {
			// Prediction:
			// - This frame: unsure
			weight += 1;
			// - Next frame: needed
			weight += block->iUseCount;
		}
	}

	return weight;
}

//---------------------------------------------------------------------------
// Simple block allocator
static struct SVramBlock *
_blockAlloc(unsigned int size)
{
	struct SVramBlock * block = NULL;
	unsigned int weight = 0;

	// Locate free block
	for (block = head; block != NULL; block = block->pNext) {
		if ((block->tex == NULL) && (block->iSize >= size)) {
			// Free block found (first fit)
			break;
		}
	}

	while (block == NULL) {
		// Free blocks starting with the least used textures
		for (block = head; block != NULL; block = block->pNext) {
			if ((block->tex != NULL) && (_blockGetWeight(block) <= weight)) {
				// Free block
				block->tex = NULL;
				block = _blockMergeFree(block);
				if (block->iSize >= size) {
					// Free block found (created)
					break;
				}
			}
		}
		weight++;
	}

	// Split the block into the right size
	block = _blockSplitFree(block, size);

	return block;
}

//---------------------------------------------------------------------------
// Public functions

//---------------------------------------------------------------------------
void
gsKit_TexManager_init(GSGLOBAL * gsGlobal)
{
	struct SVramBlock * block = head;

	// Delete all blocks (if present)
	while(block != NULL)
		block = _blockRemove(block);

	// Allocate the initial free block
	head = _blockCreate(gsGlobal->CurrentPointer, (4*1024*1024) - gsGlobal->CurrentPointer);
}

//---------------------------------------------------------------------------
void
gsKit_TexManager_setmode(GSGLOBAL * gsGlobal, enum ETransferMode mode)
{
	trmode = mode;
}

//---------------------------------------------------------------------------
unsigned int
gsKit_TexManager_bind(GSGLOBAL * gsGlobal, GSTEXTURE * tex)
{
	struct SVramBlock * block;
	unsigned int ttransfer = 0;
	unsigned int ctransfer = 0;
	unsigned int tsize;
	unsigned int csize = 0;
	unsigned int cwidth = 16;
	unsigned int cheight = 16;

	// Locate texture
	for (block = head; block != NULL; block = block->pNext) {
		if(block->tex == tex)
			break;
	}

	tsize = gsKit_texture_size(tex->Width, tex->Height, tex->PSM);
	if (tex->Clut != NULL) {
		cwidth  = (tex->PSM == GS_PSM_T8) ? 16 : 8;
		cheight = (tex->PSM == GS_PSM_T8) ? 16 : 2;
		csize   = gsKit_texture_size(cwidth, cheight, tex->ClutPSM);
	}

	// Allocate new block if not already loaded
	if (block == NULL) {
		block = _blockAlloc(tsize + csize);
		block->tex = tex;
		block->iUseCount = 0;
		block->iUseCountPrev = 1;

		tex->Vram = 0;
		tex->VramClut = 0;
	}

	// (Re-)transfer texture if invalidated
	if (tex->Vram == 0)
		ttransfer = 1;

	// (Re-)transfer clut if invalidated
	if ((tex->Clut) && (tex->VramClut == 0))
		ctransfer = 1;

	if (ttransfer) {
		tex->Vram = block->iStart;
		gsKit_setup_tbw(tex);
		SyncDCache(tex->Mem, (u8 *)(tex->Mem) + tsize);
		if (trmode == ETM_INLINE)
			gsKit_texture_send_inline(gsGlobal, tex->Mem, tex->Width, tex->Height, tex->Vram, tex->PSM, tex->TBW, tex->Clut ? GS_CLUT_TEXTURE : GS_CLUT_NONE);
		else
			gsKit_texture_send(tex->Mem, tex->Width, tex->Height, tex->Vram, tex->PSM, tex->TBW, tex->Clut ? GS_CLUT_TEXTURE : GS_CLUT_NONE);
	}

	if (ctransfer) {
		tex->VramClut = block->iStart + tsize;
		SyncDCache(tex->Clut, (u8 *)(tex->Clut) + csize);
		if (trmode == ETM_INLINE)
			gsKit_texture_send_inline(gsGlobal, tex->Clut, cwidth, cheight, tex->VramClut, tex->ClutPSM, 1, GS_CLUT_PALLETE);
		else
			gsKit_texture_send(tex->Clut, cwidth, cheight, tex->VramClut, tex->ClutPSM, 1, GS_CLUT_PALLETE);
	}

	block->iUseCount++;

	return (ttransfer|ctransfer);
}

//---------------------------------------------------------------------------
void
gsKit_TexManager_invalidate(GSGLOBAL * gsGlobal, GSTEXTURE * tex)
{
	tex->Vram = 0;
	tex->VramClut = 0;
}

//---------------------------------------------------------------------------
void
gsKit_TexManager_free(GSGLOBAL * gsGlobal, GSTEXTURE * tex)
{
	struct SVramBlock * block;

	// Locate texture
	for (block = head; block != NULL; block = block->pNext) {
		if(block->tex == tex) {
			// Free block
			block->tex = NULL;
			block = _blockMergeFree(block);
			tex->Vram = 0;
			tex->VramClut = 0;
			break;
		}
	}
}

//---------------------------------------------------------------------------
void
gsKit_TexManager_nextFrame(GSGLOBAL * gsGlobal)
{
	struct SVramBlock * block;

	// Register use count
	for(block = head; block != NULL; block = block->pNext) {
		if(block->tex != NULL) {
			block->iUseCountPrev = block->iUseCount;
			block->iUseCount = 0;
		}
	}
}
