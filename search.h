#ifndef SEARCH_H
#define SEARCH_H
#include "pattern.h"
#include "tuplenet.h"
#include "transposition.h"

#define MIN_STATE 0

// The search algorithm
class Search {
private:
	typedef Tuples<Feature<0, 1, 2, 3, 4, 5>,
			 Feature<4, 5, 6, 7, 8, 9>,
			 Feature<0, 1, 2, 4, 5, 6>,
			 Feature<4, 5, 6, 8, 9, 10>> tuple_network;
	static TranspositionTable transposition;

	// Probability threshold for node cut-off
	float prob_treshold = 0.0001;

	int state_evaled = 0;

	// Max node of expectimax tree search
	float ExpectimaxMove(board_t b, int depth, float prob) {
		state_evaled++;
		float max = 0;
		for (int i = 0; i < 4; i++) {
			board_t moved = move(b, i);
			if (moved == b) continue;
			max = std::max(max, ExpectimaxSpawn(moved, depth - 1, prob));
		}
		return max;
	}

	// Chance node of expectimax tree search
	float ExpectimaxSpawn(board_t b, int depth, float prob) {
		float expect = 0;
		if (depth <= 0 || prob < prob_treshold) return tuple_network::Estimate(b);
		int current_evaled = state_evaled;
		state_evaled += transposition.Lookup(b, depth, &expect);
		if (state_evaled > current_evaled) return expect;
		expect = 0;
		board_t mask = EmptyPos(b);
		int empty = int((mask * 0x1111111111111111ull) >> 60);
		prob /= (float)empty;
		float prob2 = prob * 0.9;
		float prob4 = prob * 0.1;
		while (mask) {
			board_t tile = mask & (~mask + 1);
			expect += ExpectimaxMove(b | tile, depth, prob2) * 0.9;
			expect += ExpectimaxMove(b | (tile << 1), depth, prob4) * 0.1;
			mask ^= tile;
		}
		expect /= (float)empty;
		transposition.Update(b, depth, expect, state_evaled - current_evaled);
		return expect;
	};
public:
	Search() {tuple_network::Load("weights.bin");}
	
	float operator()(board_t b, int dir) {
		board_t new_board = move(b, dir);
		if (new_board == b) return 0;
		state_evaled = 0;
		int last_evaled = -1;
		int depth = 0;
		prob_treshold = 1.0f / (1 << (4 * depth));
		float value = ExpectimaxSpawn(new_board, depth, 1);
		while (state_evaled < MIN_STATE && state_evaled > last_evaled) {
			depth++;
			last_evaled = state_evaled;
			state_evaled = 0;
			prob_treshold = 1.0f / (1 << (4 * depth));
			value = ExpectimaxSpawn(new_board, depth, 1);
		}
		return value;
	}
};

TranspositionTable Search::transposition;

#endif // SEARCH_H
