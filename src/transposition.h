#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H

#include <chrono>
#include <random>

class TranspositionTable {
   public:
    TranspositionTable() {
        std::mt19937 mt(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<int> distribution(0, 0x3fffff);
        for (auto i = 0; i < 256; i++) zMap[i] = distribution(mt);
    }

    bool Lookup(board_t board, int depth, float* score) {
        Entry entry = entries[Hash(board)];
        if (entry.board == board && entry.depth >= depth) {
            *score = entry.score;
            return true;
        }
        return false;
    }

    void Update(board_t board, int depth, float score) {
        Entry& entry = entries[Hash(board)];
        entry.board = board;
        entry.depth = depth;
        entry.score = score;
    }

    void CLear() {
        for (auto i = 0; i < 0x400000; ++i) entries[i].board = 0;
    }

   private:
    struct Entry {
        board_t board;
        float score;
        int depth;
    };

    Entry entries[0x400000];

    int zMap[256];

    int Hash(board_t x) {
        auto value = 0;
        for (auto i = 0; i < 16; ++i) {
            value ^= zMap[(i << 4) | (x & 0xf)];
            x >>= 4;
        }
        return value;
    }
};

#endif
