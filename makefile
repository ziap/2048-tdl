STRUCTURE?=nw4x6
ENABLE_TC?=true
CXX?=g++

OPTIONS=-DSTRUCTURE=$(STRUCTURE) -DENABLE_TC=$(ENABLE_TC) -O3 -mtune=native -std=c++17 -DFILE_NAME=\"$(STRUCTURE)/weights.bin\" -mbmi2
all:
	$(CXX) tdl2048.cpp -o train $(OPTIONS)
	$(CXX) agent.cpp -o agent $(OPTIONS) -pthread
	mkdir -p $(STRUCTURE)
