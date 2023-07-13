#ifndef BOARD_H
#define BOARD_H

#include <iomanip>
#include <iostream>

#include "math.h"

namespace board {

using t = u64;

inline void print(t b) {
  std::cout << "+-------+-------+-------+-------+\n";
  for (auto i = 60; i >= 0; i -= 4) {
    auto tile = (1u << math::pext64(b, 0xfull << i)) & -2;
    if (tile) std::cout << '|' << std::setw(6) << tile << ' ';
    else
      std::cout << "|       ";
    if (!(i % 16)) std::cout << "|\n+-------+-------+-------+-------+\n";
  }
}

constexpr t empty_pos(t b) {
  b |= (b >> 2) & 0x3333333333333333ull;
  b |= (b >> 1);
  return ~b & 0x1111111111111111ull;
}

inline t add_tile(t b, math::random &rng) {
  auto mask = empty_pos(b);
  auto empty = math::popcnt(mask);
  auto idx = rng() % empty;
  for (auto i = 0; i < idx; i++) mask &= mask - 1;
  auto tile = mask & -mask;

  return b | (tile << (rng() % 10 == 0));
}

constexpr u32 max_tile(t b) {
  auto max = 0u;
  for (; b; b >>= 4) max = std::max(max, u32(b & 0xf));
  return max;
}

constexpr u16 reverse_row(u16 row) {
  return u16(
    (row >> 12) | ((row >> 4) & 0x00f0) | ((row << 4) & 0x0f00) | (row << 12)
  );
}

constexpr t flip(t b) {
  return (
    ((b & 0x000000000000ffffull) << 48) | ((b & 0x00000000ffff0000ull) << 16) |
    ((b & 0x0000ffff00000000ull) >> 16) | ((b & 0xffff000000000000ull) >> 48)
  );
}

constexpr t transpose(t x) {
  t b = (x ^ (x >> 12)) & 0x0000f0f00000f0f0ull;
  x ^= b ^ (b << 12);
  b = (x ^ (x >> 24)) & 0x00000000ff00ff00ull;
  x ^= b ^ (b << 24);
  return x;
}

}  // namespace board

#endif
