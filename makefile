STRUCTURE?=nw4x6
CXX?=g++
ENABLE_GUI?=true
STRUCTURES=nw5x4 nw4x5 nw6x5 nw4x6 nw5x6 nw8x6

ifeq ($(ENABLE_GUI), true)
WEBVIEW_DEPS=`pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0`
GUI_OBJ=gui.o
else
WEBVIEW_DEPS=
GUI_OBJ=
endif

DEFINES=-DSTRUCTURE=$(STRUCTURE) \
        -DFILE_NAME=\"$(STRUCTURE)/weights.bin\" \
        -DGUI=$(ENABLE_GUI)
OPTIMIZATIONS=-O3 -march=native -mtune=native
FEATURES=-std=c++17 -mbmi -mbmi2 -mavx -mavx2 -pthread
EXTRAS?=

OPTS=$(DEFINES) $(OPTIMIZATIONS) $(FEATURES) $(EXTRAS)

ifneq ($(filter $(STRUCTURE), $(STRUCTURES)),)
all: $(STRUCTURE) $(GUI_OBJ) src/webview.h
	$(CXX) $(OPTS) $(WEBVIEW_DEPS) -o 2048 src/2048.cpp $(GUI_OBJ)
else
all:
	@echo "Structure $(STRUCTURE) does not exists"
	@echo Available structures: $(STRUCTURES)
endif

$(STRUCTURE):
	mkdir -p $(STRUCTURE)

$(GUI_OBJ):
	echo "const char* html = R\"($$(cat src/gui.html))\";" | g++ -xc++ -c -o $(GUI_OBJ) -

src/webview.h:
	curl https://raw.githubusercontent.com/webview/webview/master/webview.h -o src/webview.h

clean:
	rm -f 2048 gui.o
