#!/bin/sh
# I just used a sh file since makefiles have a weird syntax and it's just faster to write a sh file...

set -e #exit on error

OUTNAME=neem
INCLUDEDIR=include
BINDIR=binfiles
SRCDIR=src

#Compile lib
for filename in ./${SRCDIR}/*.cpp; do
	g++ -g -c "$filename" -I$INCLUDEDIR -o ${BINDIR}/`basename ${filename%.*}`.o
done
gcc -g -c `find ./*.c` -I$INCLUDEDIR -o ${BINDIR}/main.o
g++ -g `find ${BINDIR}/*` $FLAGS -o $OUTNAME

gdb --args ./neem.exe main.neem