//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsInit.c - GS initialization and configuration routines.
//
// Parts taken from ooPo's ee-syscalls.txt
// http://www.oopo.net/consoledev/files/ee-syscalls.txt
//

#include "gsKit.h"
#include "kernel.h"

int gsKit_init(unsigned int interlace, unsigned int mode, unsigned int field)
{
	__asm__ __volatile__("
		# SetGsCrt
		li  $3, 0x02;
		syscall;
		nop;");

	return 0;
}


