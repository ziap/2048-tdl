#include "board.h"
#include "search.h"
#include "seed.h"
#include <iostream>
#include <numeric>

std::vector<double> speeds;
std::vector<unsigned> scores;
int rate[16] = { 0 };
int games = 1000;

int main() {
	Search search;
	long long seed = RandomSeed();
	srand(seed);
	std::cout << "seed = " << seed << '\n';
	for (int n = 1; n <= games; n++) {
        int moves = 0;
		std::cout << '\r' << n << '/' << games;
		std::cout.flush();
        auto start = std::chrono::high_resolution_clock::now();
		board_t board = AddTile(AddTile(0));
		int score = 0;
		for (;;) {
			float best = 0;
			int dir = -1;
			for (int i = 0; i < 4; i++) {
				float val = search(board, i);
				if (val > best) {
					best = val;
					dir = i;
				}
			}
			if (dir < 0) break;
			moves++;
			score += move.Score(board, dir);
			board = AddTile(move(board, dir));
		}
        auto end = std::chrono::high_resolution_clock::now();
        speeds.push_back((double)moves * 1e9 / (double)std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
		scores.push_back(score);
		rate[MaxRank(board)]++;
		//tdl.UpdateEpisode();
	}
	std::cout << '\n';
	std::cout << "average score: " << std::accumulate(scores.begin(), scores.end(), 0) / games << '\n';
	std::cout << "average speed: " << std::accumulate(speeds.begin(), speeds.end(), 0.0) / (double)games << " moves per second\n";
	int accu = 0;
	for (int i = 15; i > 0; i--) {
		if (!rate[i]) continue;
		accu += rate[i];
		std::cout << '\t' << (1 << i) << '\t' << accu * 100.f / float(games) << "%\n";
	}
}
