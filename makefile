STRUCTURE?=nw4x6
all:
	g++ tdl2048.cpp -o train -DSTRUCTURE=$(STRUCTURE) -O3 -std=c++17
	g++ agent.cpp -o agent -DSTRUCTURE=$(STRUCTURE) -O3 -std=c++17 -pthread