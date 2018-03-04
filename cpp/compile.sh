#!/bin/sh
# I just used a sh file since makefiles have a weird syntax and it's just faster to write a sh file...

OUTNAME=neem
INCLUDEDIR=include
BINDIR=binfiles
SRCDIR=src

if [ "$1" == "release" ]; then
	OUTNAME=${OUTNAME}-release
	FLAGS="${FLAGS} -O2 "
fi

#Compile lib
for filename in ./${SRCDIR}/*.cpp; do
	g++ -c "$filename" -I$INCLUDEDIR -o ${BINDIR}/`basename ${filename%.*}`.o
done
gcc -c `find ./*.c` -I$INCLUDEDIR -o ${BINDIR}/main.o
g++ `find ${BINDIR}/*` $FLAGS -o $OUTNAME
strip $OUTNAME

if [ "$1" == "run" ]; then
	./$OUTNAME main.neem
fi