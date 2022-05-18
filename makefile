STRUCTURE?=nw4x6
CXX?=g++
ENABLE_GUI?=true
STRUCTURES=nw5x4 nw4x5 nw6x5 nw4x6 nw5x6 nw8x6

ifeq ($(ENABLE_GUI), true)
WEBVIEW_DEPS=`pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0`
else
WEBVIEW_DEPS=
endif

DEFINES=-DSTRUCTURE=$(STRUCTURE)\
	-DFILE_NAME=\"$(STRUCTURE)/weights.bin\"\
	-DGUI=$(ENABLE_GUI)
OPTIMIZATIONS=-O3 -march=native -mtune=native
FEATURES=-mbmi -mbmi2 -mavx -mavx2 -pthread
EXTRAS?=

OPTS=$(DEFINES) $(OPTIMIZATIONS) $(FEATUERS) $(EXTRAS)

ifneq ($(filter $(STRUCTURE), $(STRUCTURES)),)
all:
	echo "const char* html = R\"($$(cat src/gui.html))\";" | g++ -xc++ -c -o gui.o -
	$(CXX) $(OPTS) $(WEBVIEW_DEPS) -o 2048 src/2048.cpp gui.o
	mkdir -p $(STRUCTURE)
else
all:
	@echo "Structure $(STRUCTURE) does not exists"
	@echo Available structures: $(foreach struct, $(STRUCTURES), -$(struct))
endif