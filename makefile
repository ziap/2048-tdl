STRUCTURE?=nw4x6
ENABLE_TC?=true
CXX?=g++

WEBVIEW_DEPS=`pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0`
OPTIONS=-DSTRUCTURE=$(STRUCTURE) -DENABLE_TC=$(ENABLE_TC) -O3 -mtune=native -std=c++17 -DFILE_NAME=\"$(STRUCTURE)/weights.bin\" -mbmi2
all:
	$(CXX) train.cpp -o train $(OPTIONS)
	$(CXX) agent.cpp $(WEBVIEW_DEPS) -o agent $(OPTIONS) -pthread
	mkdir -p $(STRUCTURE)
