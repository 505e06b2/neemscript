#!/bin/sh
# I just used a sh file since makefiles have a weird syntax and it's just faster to write a sh file...

set -e #exit on error

OUTNAME=neem
INCLUDEDIR=include
BINDIR=binfiles
SRCDIR=src
PREFIX=/cygdrive/c/PortableMinGW/MinGW/bin

if [ "$1" == "release" ]; then
	OUTNAME=${OUTNAME}-release
	FLAGS="${FLAGS} -O2 "
fi

#Compile lib
for filename in ./${SRCDIR}/*.cpp; do
	${PREFIX}/g++ -c "$filename" -I$INCLUDEDIR -o ${BINDIR}/`basename ${filename%.*}`.o
done
${PREFIX}/gcc -c `find ./*.c` -I$INCLUDEDIR -o ${BINDIR}/main.o
${PREFIX}/g++ `find ${BINDIR}/*` $FLAGS -o $OUTNAME
strip $OUTNAME

if [ "$1" == "run" ]; then
	./$OUTNAME main.neem
fi