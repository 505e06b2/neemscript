SCRIPTNAME=main.neem
OUTNAME=neem
INCLUDEDIR=include
BINDIR=binfiles
SRCDIR=src
CC=gcc
CXX=g++

ifeq ($(UNAME),Linux)
TARGET = $(OUTNAME)
RMCMD = rm $(BINDIR)/*.o && rm $(TARGET)
else
TARGET = $(OUTNAME).exe
RMCMD = cmd \/c "del $(BINDIR)\\*.o && del $(TARGET)"
endif

$(TARGET): $(patsubst $(SRCDIR)%,$(BINDIR)%,$(patsubst %.cpp,%.o,$(wildcard $(SRCDIR)/*.cpp))) $(BINDIR)/main.o
	$(CXX) -O2 $^ -o $@
	strip $@

$(BINDIR)/main.o: $(SRCDIR)/main.c $(INCLUDEDIR)/neemWrapper.h
	$(CC) -O2 -c $< -I$(INCLUDEDIR) -o $@
	
$(BINDIR)/%.o: $(SRCDIR)/%.cpp $(INCLUDEDIR)/neem.h
	$(CXX) -O2 -c $< -I$(INCLUDEDIR) -o $@

.PHONY: run clean

ifneq (,$(findstring clean,$(MAKECMDGOALS)))
.NOTPARALLEL:
endif
clean:
	$(RMCMD)
	
run: $(TARGET)
	./$^ $(SCRIPTNAME)