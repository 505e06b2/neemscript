#!/bin/sh
# I just used a sh file since makefiles have a weird syntax and it's just faster to write a sh file...

OUTNAME=neem
INCLUDEDIR=include
BINDIR=binfiles
SRCDIR=src

#Compile lib
for filename in ./${SRCDIR}/*.cpp; do
	g++ -c "$filename" -I$INCLUDEDIR -o ${BINDIR}/`basename ${filename%.*}`.o
done
gcc -c `find ./*.c` -I$INCLUDEDIR -o ${BINDIR}/main.o
g++ `find ${BINDIR}/*` -o $OUTNAME
strip $OUTNAME

if [ "$1" == "run" ]; then
	./$OUTNAME main.neem
fi