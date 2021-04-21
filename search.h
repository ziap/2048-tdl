#ifndef SEARCH_H
#define SEARCH_H
#include "tuplenet.h"
#include "transposition.h"

#define MIN_STATE 0

// The search algorithm
class Search {
private:
    int min_depth = 0;

    typedef Tuples<Feature<0, 1, 2, 3, 4, 5>,
                    Feature<4, 5, 6, 7, 8, 9>,
                    Feature<0, 1, 2, 4, 5, 6>,
                    Feature<4, 5, 6, 8, 9, 10>> tuple_network;
    static TranspositionTable transposition;

    int state_evaled = 0;

    // Max node of expectimax tree search
    float ExpectimaxMove(board_t b, int depth) {
        state_evaled++;
        float max = 0;
        for (int i = 0; i < 4; i++) {
            board_t moved = move(b, i);
            if (moved == b) continue;
            max = std::max(max, ExpectimaxSpawn(moved, depth));
        }
        return max;
    }

    // Chance node of expectimax tree search
    float ExpectimaxSpawn(board_t b, int depth) {
        float expect = 0;
        if (depth <= 0) return tuple_network::Estimate(b);
        int current_evaled = state_evaled;
        state_evaled += transposition.Lookup(b, depth, &expect);
        if (state_evaled > current_evaled) return expect;
        expect = 0;
        board_t mask = EmptyPos(b);
        int empty = int((mask * 0x1111111111111111ull) >> 60);
        while (mask) {
            board_t tile = mask & (~mask + 1);
            expect += ExpectimaxMove(b | tile, depth - 1) * 0.9;
            expect += ExpectimaxMove(b | (tile << 1), depth - 1) * 0.1;
            mask ^= tile;
        }
        expect /= (float)empty;
        transposition.Update(b, depth, expect, state_evaled - current_evaled);
        return expect;
    };
public:
    Search(int depth) {
        tuple_network::Load("weights.bin");
        min_depth = depth;
    }
    
    float operator()(board_t b, int dir) {
        board_t new_board = move(b, dir);
        if (new_board == b) return 0;
        state_evaled = 0;
        int last_evaled = -1;
        int depth = min_depth;
        float value = ExpectimaxSpawn(new_board, depth);
        while (state_evaled < MIN_STATE && state_evaled > last_evaled) {
            depth++;
            last_evaled = state_evaled;
            state_evaled = 0;
            value = ExpectimaxSpawn(new_board, depth);
        }
        return value;
    }
};

TranspositionTable Search::transposition;

#endif // SEARCH_H
