#!/bin/bash

# This should be an acual 'make' file...

FILES=( main email adsl serial updater ping iface config )

COMPILER="gcc"
COMPILE_OPTIONS="-c -Wall -Wextra -Wstrict-prototypes -Wunused-result -O3 -std=c99 -fmessage-length=0"

LINKER="gcc"
LINKER_OPTIONS="-L/usr/lib -lpthread -lcurl -loping -s -Wl,--gc-sections"

OUT="btnetmon"



FILES_LINKER=""
for i in "${FILES[@]}"
do
	$COMPILER $COMPILE_OPTIONS -o $i.o $i.c
	FILES_LINKER="$FILES_LINKER $i.o"
done

$LINKER $FILES_LINKER -o $OUT $LINKER_OPTIONS
