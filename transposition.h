#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H
#include <random>
#include <chrono>

class TranspositionTable
{
public:
    TranspositionTable() {
        std::mt19937 mt(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<int> distribution(0, 0x3fffff);
        for (int i = 0; i < 256; i++) zMap[i] = distribution(mt);
    }
    int Lookup(board_t board, int depth, float* score) {
        Entry entry = entries[Hash(board)];
        if (entry.board == board && entry.depth >= depth) {
            *score = entry.score;
            return std::pow(entry.moves, (float)depth / (float)entry.depth);
        }
        return 0;
    }
    void Update(board_t board, int depth, float score, int moves) {
        Entry& entry = entries[Hash(board)];
        entry.board = board;
        entry.depth = depth;
        entry.score = score;
        entry.moves = moves;
    }
    void CLear() {
        for (int i = 0; i < 0x400000; ++i) entries[i].board = 0;
    }
private:
    struct Entry {
        board_t board;
        float score;
        int depth;
        int moves;
    };
    Entry entries[0x400000];
    int zMap[256];
    int Hash(board_t x) {
        int value = 0;
        for (int i = 0; i < 16; ++i) {
            value ^= zMap[(i << 4) | (x & 0xf)];
            x >>= 4;
        }
        return value;
    }
};

#endif // TRANSPOSITION_H