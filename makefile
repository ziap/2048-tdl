STRUCTURE?=nw4x6
CXX?=g++
ENABLE_GUI?=true

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

all:
	objcopy --input binary --output elf64-x86-64 --binary-architecture i386:x86-64 src/gui.html gui.o
	$(CXX) $(OPTS) $(WEBVIEW_DEPS) -o 2048 src/2048.cpp gui.o
	mkdir -p $(STRUCTURE)