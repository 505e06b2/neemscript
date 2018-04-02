SCRIPTNAME=main.neem
OUTNAME=neem
INCLUDEDIR=include
BINDIR=binfiles
SRCDIR=src
PREFIX=

ifeq ($(UNAME),Linux)
TARGET = $(OUTNAME)
RMCMD = rm $(BINDIR)/*.o && rm $(TARGET)
else
TARGET = $(OUTNAME).exe
RMCMD = cmd \/c "del $(BINDIR)\\*.o && del $(TARGET)"
endif

ifneq (,$(findstring mingw,$(MAKECMDGOALS)))
PREFIX=/cygdrive/c/PortableMinGW/MinGW/bin/
endif

$(TARGET): $(patsubst $(SRCDIR)%,$(BINDIR)%,$(patsubst %.cpp,%.o,$(wildcard $(SRCDIR)/*.cpp))) $(BINDIR)/main.o
	$(PREFIX)g++ -O2 $^ -o $@
	strip $@

$(BINDIR)/main.o: $(SRCDIR)/main.c $(INCLUDEDIR)/neemWrapper.h
	$(PREFIX)gcc -O2 -c $< -I$(INCLUDEDIR) -o $@
	
$(BINDIR)/%.o: $(SRCDIR)/%.cpp $(INCLUDEDIR)/neem.h
	$(PREFIX)gcc -O2 -c $< -I$(INCLUDEDIR) -o $@

.PHONY: mingw run clean
mingw: $(TARGET)

.NOTPARALLEL:
clean:
	$(RMCMD)
	
run: $(TARGET)
	./$^ $(SCRIPTNAME)