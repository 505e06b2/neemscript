SCRIPTNAME=main.neem
OUTNAME=neem
INCLUDEDIR=include
BINDIR=binfiles
SRCDIR=src
PREFIX=

ifeq ($(UNAME),Linux)
TARGET = neem
RMCMD = rm $(BINDIR)/*.o
else
TARGET = neem.exe
endif

ifeq ($(MAKECMDGOALS),mingw)
PREFIX=/cygdrive/c/PortableMinGW/MinGW/bin/
endif

$(TARGET): $(patsubst $(SRCDIR)%,$(BINDIR)%,$(patsubst %.cpp,%.o,$(wildcard $(SRCDIR)/*.cpp))) main.o
	$(PREFIX)g++ -O2 $^ -o $@
	strip $@

main.o: main.c $(INCLUDEDIR)/neemWrapper.h
	$(PREFIX)gcc -O2 -c $< -I$(INCLUDEDIR) -o $@
	
$(BINDIR)/%.o: $(SRCDIR)/%.cpp $(INCLUDEDIR)/neem.h
	$(PREFIX)gcc -O2 -c $< -I$(INCLUDEDIR) -o $@

.PHONY: mingw run clean
clean:
	rm $(BINDIR)/*.o

mingw: $(TARGET)

run: $(TARGET)
	./$^ $(SCRIPTNAME)