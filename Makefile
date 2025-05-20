#  ____     ___ |    / _____ _____
# |  __    |    |___/    |     |
# |___| ___|    |    \ __|__   |     gsKit Open Source Project.
# ----------------------------------------------------------------------
# Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
# Licenced under Academic Free License version 2.0
# Review gsKit README & LICENSE files for further details.

.PHONY: all install

all:
	cmake -S . -B build -Wno-dev "-DCMAKE_C_COMPILER=mips64r5900el-ps2-elf-gcc" "-DCMAKE_CXX_COMPILER=mips64r5900el-ps2-elf-g++" "-DCMAKE_TOOLCHAIN_FILE=${PS2DEV}/share/ps2dev.cmake" "-DCMAKE_INSTALL_PREFIX=${PS2SDK}/ports" -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo "-DCMAKE_PREFIX_PATH=${PS2SDK}/ports"
	cmake --build build

install: all
	cmake --build build --target install

clean:
	rm -rf build
