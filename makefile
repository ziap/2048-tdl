STRUCTURE?=nw4x6
ENABLE_TC?=true
ENABLE_GUI?=true
CXX?=g++

ifeq ($(ENABLE_GUI), true)
WEBVIEW_DEPS=`pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0`
else
WEBVIEW_DEPS=
endif

OPTIONS=-DSTRUCTURE=$(STRUCTURE) -DENABLE_TC=$(ENABLE_TC) -DGUI=$(ENABLE_GUI) -O3 -mtune=native -std=c++17 -DFILE_NAME=\"$(STRUCTURE)/weights.bin\" -mbmi2 -pthread
all:
	$(CXX) src/train.cpp -o train $(OPTIONS)
	$(CXX) src/agent.cpp $(WEBVIEW_DEPS) -o agent $(OPTIONS)
	mkdir -p $(STRUCTURE)