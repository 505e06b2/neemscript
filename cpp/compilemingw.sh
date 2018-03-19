#!/bin/sh
# I just used a sh file since makefiles have a weird syntax and it's just faster to write a sh file...

set -e #exit on error

OUTNAME=neem
INCLUDEDIR=include
BINDIR=binfiles
SRCDIR=src
PREFIX=/cygdrive/c/PortableMinGW/MinGW/bin

#Compile lib
for filename in ./${SRCDIR}/*.cpp; do
	${PREFIX}/g++ -O2 -c "$filename" -I$INCLUDEDIR -o ${BINDIR}/`basename ${filename%.*}`.o
done
${PREFIX}/gcc -O2 -c `find ./*.c` -I$INCLUDEDIR -o ${BINDIR}/main.o
${PREFIX}/g++ -O2 `find ${BINDIR}/*` -o $OUTNAME
strip $OUTNAME

if [ "$1" == "run" ]; then
	./$OUTNAME main.neem
fi