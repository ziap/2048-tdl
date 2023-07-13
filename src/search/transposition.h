#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H

#include <random>

#include "../core/board.h"
#include "../core/math.h"

template <u32 size>
class transposition {
  struct entry_t {
    board::t board = 0;
    float score = 0;
    u32 depth = 0;
  };

  entry_t *entries;

  u32 z_map[256];

  u32 hash(board::t x) {
    auto value = 0u;
    for (auto i = 0u; i < 256u; i += 16) {
      value ^= z_map[i | (x & 0xf)];
      x >>= 4;
    }
    return value;
  }

 public:
  transposition() {
    entries = new entry_t[size];

    std::mt19937 mt(math::generate_seed());
    std::uniform_int_distribution<u32> dist(0, size - 1);
    for (auto i = 0u; i < 256u; i++) z_map[i] = dist(mt);
  }

  ~transposition() { delete[] entries; }

  transposition(const transposition &) = delete;
  transposition &operator=(const transposition &) = delete;
  transposition(const transposition &&) = delete;
  transposition &operator=(const transposition &&) = delete;

  bool lookup(board::t board, u32 depth, float &score) {
    auto entry = entries[hash(board)];
    if (entry.board == board && entry.depth >= depth) {
      score = entry.score;
      return true;
    }
    return false;
  }

  float update(board::t board, u32 depth, float score) {
    auto &entry = entries[hash(board)];
    entry.board = board;
    entry.depth = depth;
    entry.score = score;
    return score;
  }
};

#endif
