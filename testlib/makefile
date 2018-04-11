MINGWPREFIX=/cygdrive/c/PortableMinGW/MinGW/bin/

all: test.dll test.so

test.dll: test.c
	$(MINGWPREFIX)gcc -shared -O2 $^ -o $@
	strip $@
	
test.so: test.c
	gcc -shared -O2 $^ -o $@
	strip $@