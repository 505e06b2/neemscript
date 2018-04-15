UNAME := $(shell uname)
ifneq ($(UNAME),)
#Unix
TARGET = libtest.so
else
#Probably windows
TARGET = test.dll
endif

$(TARGET): test.c
	gcc -shared -O2 $^ -o $@
	strip $@