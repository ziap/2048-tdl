#ifndef BOARD_H
#define BOARD_H

#include <iomanip>
#include <iostream>

#include "math.h"

namespace board {

using t = unsigned long long;

inline void print(t b) {
  std::cout << "+-------+-------+-------+-------+\n";
  for (int i = 60; i >= 0; i -= 4) {
    math::u64 tile = (1 << math::pext64(b, 0xfull << i)) & -2;
    if (tile) std::cout << '|' << std::setw(6) << tile << ' ';
    else
      std::cout << "|       ";
    if (!(i % 16)) std::cout << "|\n+-------+-------+-------+-------+\n";
  }
}

inline t empty_pos(t b) {
  b |= (b >> 2) & 0x3333333333333333ull;
  b |= (b >> 1);
  return ~b & 0x1111111111111111ull;
}

inline t add_tile(t b, math::random &rng) {
  size_t num_empty = 0;
  t tiles[16] = {0};
  t mask = empty_pos(b);

  while (mask) {
    t tile = mask & (~mask + 1);
    tiles[num_empty++] = tile;
    mask ^= tile;
  }

  return b | ((tiles[rng() % num_empty]) << (rng() % 10 == 0));
}

inline math::u32 max_tile(t b) {
  math::u32 max = 0;
  for (; b; b >>= 4) max = std::max(max, math::u32(b & 0xf));
  return max;
}

inline math::u16 reverse_row(math::u16 row) {
  return math::u16(
    (row >> 12) | ((row >> 4) & 0x00f0) | ((row << 4) & 0x0f00) | (row << 12));
}

inline t flip(t b) {
  return (
    ((b & 0x000000000000ffffull) << 48) | ((b & 0x00000000ffff0000ull) << 16) |
    ((b & 0x0000ffff00000000ull) >> 16) | ((b & 0xffff000000000000ull) >> 48));
}

inline t transpose(t x) {
  t b = (x ^ (x >> 12)) & 0x0000f0f00000f0f0ull;
  x ^= b ^ (b << 12);
  b = (x ^ (x >> 24)) & 0x00000000ff00ff00ull;
  x ^= b ^ (b << 24);
  return x;
}

}  // namespace board

#endif
