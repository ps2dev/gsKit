# GSKIT

```txt
  ____     ___ |    / _____ _____
 |  __    |    |___/    |     |
 |___| ___|    |    \ __|__   |              gsKit Open Source Project.
 -----------------------------------------------------------------------
 Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>.
 All rights reserved.
 Licenced under Academic Free License version 2.0
 -----------------------------------------------------------------------
```

[![CI](https://github.com/ps2dev/gsKit/workflows/CI/badge.svg)](https://github.com/ps2dev/gsKit/actions?query=workflow%3ACI)
[![CI-Docker](https://github.com/ps2dev/gsKit/workflows/CI-Docker/badge.svg)](https://github.com/ps2dev/gsKit/actions?query=workflow%3ACI-Docker)

## **ATTENTION**

If you are confused on how to start developing for PS2, see the
[getting started](https://ps2dev.github.io/#getting-started) section on
the ps2dev main page.  

## Introduction

gsKit is a library that provides a C interface to the Playstation 2
Graphics Synthesizer. It is low level in nature, implimented using the
PS2SDK and inline assembly. This project does not aim to be all
inclusive, rather a "lean and mean" interface to the GS hardware.

The following list is of things that, when complete, gsKit will be capable
of when it reaches it's mature stages. It is currently in the early
stages of development. Read the STATUS file for detailed information.

gsKit aims to provide the following functionality:

*   Multi format GS initialization. (NTSC, PAL, DTV, VESA)
*   HalfBuffer Support (`NTSC_I` and `PAL_I` modes)
*   VSync, Double Buffering, Anti Aliasing, and Z Depth Test.
*   Render Queue Support, with different drawing modes. (`GS_ONESHOT`, and `GS_PERSISTANT`)
*   Overlay and multi-context support utilizing the "Merge Circuit".
*   Basic primitives as per the Sony documentation. (Line, Tri, TriStrip, ...)
*   Extended primitives support. (Quads and GL Style Vertex Lists)
*   Simple texture loading and handling.
*   Font and printing support for FNT and TrueType formats.
*   GS debugging and diagnostic functionality.
*   C accessibility of all documented GS functions.

gsKit also includes a library named dmaKit. dmaKit provides C
routines for DMAC usage by gsKit. The aim of this library is also
for eventual inclusion in [PS2SDK](https://github.com/ps2dev/ps2sdk). For now, it will remain part of the
gsKit project while it matures.

This library is designed for integration into the PS2SDK when it
becomes mature. As such, it is also designed to link and compile
against the PS2SDK. Support for PS2LIB and other support libraries is
untested.

## Project Layout

A source or binary release of gsKit will include the following
directories:

*   `gsKit/lib` - gsKit compiled libraries.
*   `gsKit/doc` - gsKit documentation.
*   `gsKit/examples` - Example projects demonstrating use of gsKit.
*   `gsKit/ee` - ee root directory.
*   `gsKit/ee/gs` - gsKit source root.
*   `gsKit/ee/gs/include` - gsKit include files.
*   `gsKit/ee/gs/src` - gsKit source files.
*   `gsKit/ee/dma` - dmaKit source root.
*   `gsKit/ee/dma/include` - dmaKit include files.
*   `gsKit/ee/dms/src` - dmaKit source files.
*   `gsKit/vu1` - VU1 assembly files.

## Installation

1.  Install PS2Toolchain
    In order to install `gsKit`, you must have previously installed the [PS2Toolchain](https://github.com/ps2dev/ps2toolchain)
    (which automatically install also the [PS2SDK](https://github.com/ps2dev/ps2sdk))

2.  Edit your login script
    Add this to your login script (example: `~/.bash_profile`)\
    `export GSKIT=$PS2DEV/gsKit`

NOTE: You should have already defined in your login script the `$PS2DEV` variable

3.  Compile and Install
    Run the next command
    `make && make install`

## Build & Compile Examples

1. Install [CMake](https://github.com/Kitware/CMake)

2. Build & compile
    `make all`

NOTE: The examples will be in the `build` folder

## Important Notes

There are several small notes that need to be made about gsKit operation.

*   As of version 0.3, gsKit no longer uses the scratchpad, so you are free to
    use it in your application.

*   There is a default drawbuffer size of 2MB for oneshot, and 256KB for persistent.
    If they are filled beyond capacity, bad things will start to happen.
    (Compile with `-DGSKIT_DEBUG` to get warnings about it at the expense of performance
    or just uncomment section in Makefile.pref)
    You can tweak these values with the `GS_RENDER_QUEUE_(OS/PER)_POOLSIZE` macros.
    (Look in ee/gs/include/gsInit.h)

*   Most gsKit routines (prim pushing, `TEST`/`CLAMP` mode setting, etc) will queue their
    execution into the drawbuffer. Texture data sends however, are the exception to this.
    They happen immediately upon calling, however this behavior will be changed so they
    are also queued into the drawbuffer in a future version.

*   By default gsKit's active drawbuffer is Oneshot.

*   You can specify some external libraries in Makefile.pref
    (just uncomment what you need)

## Community

Links for discussion and chat are available
[here](https://ps2dev.github.io/#community).  

## Credits

See `AUTHORS`.
