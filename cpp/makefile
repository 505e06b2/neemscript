SCRIPTNAME=main.neem
OUTNAME=neem
INCLUDEDIR=include
BINDIR=binfiles
JSBINDIR=jsbinfiles
SRCDIR=src
CC ?= gcc
CXX ?= g++
CXXFLAGS ?= -std=c++11

ifeq ($(UNAME),Linux)
TARGET = $(OUTNAME)
RMCMD = rm $(BINDIR)/*.o && rm $(TARGET)
else
TARGET = $(OUTNAME).exe
RMCMD = cmd \/c "del $(BINDIR)\\*.o && del $(TARGET)"
endif

$(TARGET): $(patsubst $(SRCDIR)%,$(BINDIR)%,$(patsubst %.cpp,%.o,$(wildcard $(SRCDIR)/*.cpp))) $(BINDIR)/main.o
	$(CXX) $(CXXFLAGS) -O2 $^ -o $@
	strip $@

$(BINDIR)/main.o: $(SRCDIR)/main.c $(INCLUDEDIR)/neemWrapper.h
	$(CC) -O2 -c $< -I$(INCLUDEDIR) -o $@
	
$(BINDIR)/%.o: $(SRCDIR)/%.cpp $(INCLUDEDIR)/neem.h
	$(CXX) $(CXXFLAGS) -O2 -c $< -I$(INCLUDEDIR) -o $@

../docs/neemscript.js: $(patsubst $(SRCDIR)%,$(JSBINDIR)%,$(patsubst %.cpp,%.bc,$(wildcard $(SRCDIR)/*.cpp))) $(JSBINDIR)/webmain.bc
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -O2 $^ -I$(INCLUDEDIR) -o ../docs/neemscript.js -s ASSERTIONS=1 -s EXPORTED_FUNCTIONS='["_webmain"]' -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]'
	
$(JSBINDIR)/webmain.bc: $(SRCDIR)/webmain.c $(INCLUDEDIR)/neemWrapper.h
	$(CC) -O2 -c $< -I$(INCLUDEDIR) -o $@

$(JSBINDIR)/%.bc: $(SRCDIR)/%.cpp $(INCLUDEDIR)/neem.h
	$(CXX) $(CXXFLAGS) -O2 -c $< -I$(INCLUDEDIR) -o $@

.PHONY: run clean web

ifneq (,$(findstring clean,$(MAKECMDGOALS)))
.NOTPARALLEL:
endif
clean:
	$(RMCMD)
	
run: $(TARGET)
	./$^ $(SCRIPTNAME)
	
web: ../docs/neemscript.js