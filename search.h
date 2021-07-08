#ifndef SEARCH_H
#define SEARCH_H
#include "tuplenet.h"
#include "transposition.h"

#include <functional>
#include <thread>
#include <chrono>

// The search algorithm
template <class T>
class Search : public T {
private:

    static TranspositionTable transposition;
    // Max node of expectimax tree search
    float ExpectimaxMove(board_t b, int depth) {
        float max = 0;
        for (int i = 0; i < 4; i++) {
            board_t moved = move(b, i);
            if (moved == b) continue;
            max = std::max(max, (float)move.Score(b, i) + ExpectimaxSpawn(moved, depth));
        }
        return max;
    }

    // Chance node of expectimax tree search
    float ExpectimaxSpawn(board_t b, int depth) {
        float expect = 0;
        if (depth <= 0) return this->Estimate(b);
        if (transposition.Lookup(b, depth, &expect)) return expect;
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
        transposition.Update(b, depth, expect);
        return expect;
    };

    int SuggestMove(board_t b, int depth) {
        int dir = -1;
        float best = 0;
        for (int i = 0; i < 4; i++) {
            board_t new_board = move(b, i);
            if (new_board == b) continue;
            float value = ExpectimaxSpawn(new_board, min_depth);
            if (value > best) {
                best = value;
                dir = i;
            } 
        }
        return dir;
    }

    int SuggestMoveIterative(board_t b, int time) {
        bool stop = false;

        int dir = -1;
        std::thread([&]{
            std::this_thread::sleep_for(std::chrono::milliseconds(time));
            stop = true;
        }).detach();
        for (int depth = min_depth; !stop; depth++) {
            dir = SuggestMove(b, depth);
        }
        return dir;
    }
public:

    int min_depth = 0;

    int search_time = 0;
    
    int operator()(board_t b) {
        if (!search_time) return SuggestMove(b, min_depth);
        return SuggestMoveIterative(b, search_time);
    }
};

template<class T>
TranspositionTable Search<T>::transposition;

#endif
