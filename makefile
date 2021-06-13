STRUCTURE?=nw4x6
ENABLE_TC?=true
all:
	g++ tdl2048.cpp -o train -DSTRUCTURE=$(STRUCTURE) -DENABLE_TC=$(ENABLE_TC) -O3 -std=c++17
	g++ agent.cpp -o agent -DSTRUCTURE=$(STRUCTURE) -DENABLE_TC=$(ENABLE_TC) -O3 -std=c++17 -pthread