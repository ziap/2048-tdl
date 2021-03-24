#include "board.h"
#include "learning.h"
#include "seed.h"
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <signal.h>

Learning<Feature<0, 1, 2, 3, 4, 5>,
		 Feature<4, 5, 6, 7, 8, 9>,
		 Feature<0, 1, 2, 4, 5, 6>,
		 Feature<4, 5, 6, 8, 9, 10>> tdl(0.03125f, 0.5f); // alpha = 1 / (8 * m) seems to work 

void signal_callback_handler(int signum) {
	std::cout << '\n';
	tdl.Save("weights.bin");
   	exit(signum);
}

int main() {
	long long seed = RandomSeed();
	srand(seed);
    signal(SIGINT, signal_callback_handler);
	tdl.Load("weights.bin");
	std::cout << "seed = " << seed << '\n';
	auto start = std::chrono::high_resolution_clock::now();
	unsigned long long moves = 0;
	for (int n = 1; moves < 50000000000; n++) {
		board_t board = AddTile(AddTile(0));
		int score = 0;
		for (;;) {
			State best = tdl.SelectBestMove(board);
			tdl.path.push_back(best);
			if (best.isValid()) {
				score += best.reward;
				board = AddTile(best.after);
				moves++;
			}
			else break;
		}
		tdl.BackwardLearning();
		tdl.MakeStat(n, board, score, (double)moves / 1e10);
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
	auto duration_h = std::chrono::duration_cast<std::chrono::hours>(end - start);
	std::cout << "training done after " << duration_h.count() << " hours\n";
	std::cout << "speed = " << (float)moves * 1e9 / (float)duration.count() << " moves per second\n";
	tdl.Save("weights.bin");
}