//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsCore.c - C/Assembler implimentation of all GS instructions.
//

#include "gsKit.h"

void gsKit_DMA_Wait(unsigned int channel)
{
 asm __volatile__(" # DMA Wait
	 lui $2, 0x1000;
	 addu $2, $4;
	 loop:
	      lw $3, 0x00($2);
	      andi $3, 0x100;
	      bnez $3, loop;
	      nop;
	      jr $31;
	      nop;");
}
