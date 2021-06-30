STRUCTURE?=nw4x6
ENABLE_TC?=true
OPTIONS=-DSTRUCTURE=$(STRUCTURE) -DENABLE_TC=$(ENABLE_TC) -O3 -std=c++17 -DFILE_NAME=\"$(STRUCTURE)/weights.bin\" -mbmi2
all:
	g++ tdl2048.cpp -o train $(OPTIONS)
	g++ agent.cpp -o agent $(OPTIONS) -pthread
	mkdir -p $(STRUCTURE)