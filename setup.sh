#! /bin/sh

echo "Welcome to the setup script for gsKit."
echo "This script will attempt to automatically detect the presence of LIBPNG, LIBJPEG, LIBTIFF and ZLIB, and will set up all required environmental variables."
echo "If your libraries are not stored in $PS2SDK/ports, please specify the location of the libraries manually."

## Determine GNU Make command.
if command -v gmake >/dev/null; then
	GNUMAKE=gmake
else
	GNUMAKE=make
fi

echo "Performing pre-install cleanup."
$GNUMAKE clean --silent

echo "Libraries:"

if [ -f $PS2SDK/ports/include/libjpg.h ];
then
	echo -e "\tLIBJPEG detected."
	export LIBJPEG=$PS2SDK/ports
else
	echo -e "\tLIBJPEG not detected."
fi

if [ -f $PS2SDK/ports/include/png.h ];
then
	echo -e "\tLIBPNG detected."
	export LIBPNG=$PS2SDK/ports
else
	echo -e "\tLIBPNG not detected."
fi

if [ -f $PS2SDK/ports/include/zlib.h ];
then
	echo -e "\tZLIB detected."
	export ZLIB=$PS2SDK/ports
else
	echo -e "\tZLIB not detected."
fi

if [ -f $PS2SDK/ports/include/tiff.h ];
then
	echo -e "\tLIBTIFF detected."
	export LIBTIFF=$PS2SDK/ports
else
	echo -e "\tLIBTIFF not detected."
fi

echo "Building gsKit."
$GNUMAKE --silent
echo "Installing gsKit."
$GNUMAKE install --silent
echo "Performing post-install cleanup."
$GNUMAKE clean --silent
