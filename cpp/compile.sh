#!/bin/sh
# I just used a sh file since makefiles have a weird syntax and it's just faster to write a sh file...

set -e #exit on error

OUTNAME=neem
INCLUDEDIR=include
BINDIR=binfiles
SRCDIR=src

#Compile lib
for filename in ./${SRCDIR}/*.cpp; do
	g++ -O2 -c "$filename" -I$INCLUDEDIR -o ${BINDIR}/`basename ${filename%.*}`.o
done
gcc -O2 -c main.c -I$INCLUDEDIR -o ${BINDIR}/main.o
g++ -O2 `find ${BINDIR}/*` $FLAGS -o $OUTNAME
strip $OUTNAME

if [ "$1" == "run" ]; then
	./$OUTNAME main.neem
fi