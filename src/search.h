#ifndef SEARCH_H
#define SEARCH_H
#include <chrono>
#include <functional>
#include <thread>

#include "transposition.h"
#include "tuplenet.h"

// The search algorithm
template <class T>
class Search : public T {
   private:
    static TranspositionTable transposition;
    // Max node of expectimax tree search
    float ExpectimaxMove(board_t b, int depth) {
        auto max = 0.0f;
        for (auto i = 0; i < 4; i++) {
            board_t moved = move(b, i);
            if (moved == b) continue;
            max = std::max(max, (float)move.Score(b, i) + ExpectimaxSpawn(moved, depth));
        }
        return max;
    }

    // Chance node of expectimax tree search
    float ExpectimaxSpawn(board_t b, int depth) {
        auto expect = 0.0f;
        if (depth <= 0) return this->Estimate(b);
        if (transposition.Lookup(b, depth, &expect)) return expect;
        expect = 0;
        board_t mask = EmptyPos(b);
        auto empty = int((mask * 0x1111111111111111ull) >> 60);
        while (mask) {
            auto tile = mask & (~mask + 1);
            expect += ExpectimaxMove(b | tile, depth - 1) * 0.9;
            expect += ExpectimaxMove(b | (tile << 1), depth - 1) * 0.1;
            mask ^= tile;
        }
        expect /= (float)empty;
        transposition.Update(b, depth, expect);
        return expect;
    };

    int SuggestMove(board_t b, int depth) {
        auto dir = -1;
        auto best = 0.0f;
        for (auto i = 0; i < 4; i++) {
            board_t new_board = move(b, i);
            if (new_board == b) continue;
            auto value = ExpectimaxSpawn(new_board, min_depth);
            if (value > best) {
                best = value;
                dir = i;
            }
        }
        return dir;
    }

   public:
    int min_depth = 0;

    int search_time = 0;

    int operator()(board_t b) { return SuggestMove(b, min_depth); }
};

template <class T>
TranspositionTable Search<T>::transposition;

#endif
