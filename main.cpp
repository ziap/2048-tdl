#include "board.h"
#include "state.h"
#include "pattern.h"
#include "learning.h"
#include <iostream>

int main() {
	std::cout << "A big tuple network consists of 2 axe-shaped 6-tuples and 2 rectangular 6-tuples\n";
	Learning<Feature<0, 1, 2, 3, 4, 5>,
			 Feature<4, 5, 6, 7, 8, 9>,
			 Feature<0, 1, 2, 4, 5, 6>,
			 Feature<4, 5, 6, 8, 9, 10>> tdl(0.025f);
	for (int n = 1; n <= 5000000; n++) {
		board_t board = AddTile(AddTile(0));
		int score = 0;
		for (;;) {
			State best = tdl.SelectBestMove(board);
			tdl.path.push_back(best);
			if (best.isValid()) {
				score += best.reward;
				board = AddTile(best.after);
			}
			else break;
		}
		tdl.UpdateEpisode();
		tdl.MakeStat(n, board, score);
	}
}