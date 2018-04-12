SCRIPTNAME=main.neem
OUTNAME=neem
INCLUDEDIR=include
BINDIR=binfiles
JSBINDIR=jsbinfiles
SRCDIR=src
CC ?= gcc
OPTIMISATIONS ?= -O2
CXX ?= g++
CXXFLAGS ?= -std=gnu++11

UNAME := $(shell uname)
ifneq ($(UNAME),)
TARGET = $(OUTNAME)
RMCMD = rm $(BINDIR)/*.o && rm $(TARGET)
ICONFILE =
LDFLAGS += -ldl
else
TARGET = $(OUTNAME).exe
RMCMD = cmd \/c "del $(BINDIR)\\*.o && del $(TARGET) && del misc\\*.res"
ICONFILE = misc/icon.res
endif

$(TARGET): $(patsubst $(SRCDIR)%,$(BINDIR)%,$(patsubst %.cpp,%.o,$(wildcard $(SRCDIR)/*.cpp))) $(BINDIR)/main.o $(ICONFILE)
	$(CXX) $(CXXFLAGS) $(OPTIMISATIONS) $^ -o $@ $(LDFLAGS)
	strip $@

$(BINDIR)/main.o: $(SRCDIR)/main.c $(INCLUDEDIR)/neemWrapper.h
	$(CC) $(OPTIMISATIONS) -c $< -I$(INCLUDEDIR) -o $@
	
$(BINDIR)/%.o: $(SRCDIR)/%.cpp $(INCLUDEDIR)/neem.h
	$(CXX) $(CXXFLAGS) $(OPTIMISATIONS) -c $< -I$(INCLUDEDIR) -o $@

#MISC COMMANDS
../docs/neemscript.js: $(patsubst $(SRCDIR)%,$(JSBINDIR)%,$(patsubst %.cpp,%.bc,$(wildcard $(SRCDIR)/*.cpp))) $(JSBINDIR)/webmain.bc
	$(CXX) $(CXXFLAGS) $(OPTIMISATIONS) $^ -I$(INCLUDEDIR) -o ../docs/neemscript.js -s ASSERTIONS=1 -s EXPORTED_FUNCTIONS='["_webmain"]' -s EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap"]'
	
$(JSBINDIR)/webmain.bc: $(SRCDIR)/webmain.c $(INCLUDEDIR)/neemWrapper.h
	$(CC) $(OPTIMISATIONS) -c $< -I$(INCLUDEDIR) -o $@

$(JSBINDIR)/%.bc: $(SRCDIR)/%.cpp $(INCLUDEDIR)/neem.h
	$(CXX) $(CXXFLAGS) $(OPTIMISATIONS) -c $< -I$(INCLUDEDIR) -o $@
	
misc/icon.res: ../icon.ico misc/icon.rc
	windres misc/icon.rc -O coff -o $@

.PHONY: run runwrapper clean web

ifneq (,$(findstring clean,$(MAKECMDGOALS)))
.NOTPARALLEL:
endif
clean:
	$(RMCMD)

run: | $(TARGET) runwrapper
runwrapper: $(TARGET)
	./$^ $(SCRIPTNAME)

web: ../docs/neemscript.js