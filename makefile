STRUCTURE?=nw4x6
ENABLE_TC?=true
ENABLE_GUI?=true
CXX?=g++
STD?=c++17

ifeq ($(ENABLE_GUI), true)
WEBVIEW_DEPS=`pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0`
else
WEBVIEW_DEPS=
endif

FLAGS=-DSTRUCTURE=$(STRUCTURE) -DENABLE_TC=$(ENABLE_TC) -DGUI=$(ENABLE_GUI) -std=$(STD) -DFILE_NAME=\"$(STRUCTURE)/weights.bin\" 
OPTIM=-O3 -march=native -mtune=native
FEATS=-mbmi -mbmi2 -mavx -mavx2 -pthread

OPTIONS=$(FLAGS) $(OPTIM) $(FEATS)
all:
	$(CXX) src/train.cpp -o train $(OPTIONS)
	$(CXX) src/agent.cpp $(WEBVIEW_DEPS) -o agent $(OPTIONS)
	mkdir -p $(STRUCTURE)