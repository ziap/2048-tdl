all:
	g++ main.cpp -o 2048 -O3 -std=c++17
	g++ agent.cpp -o agent -O3 -std=c++17