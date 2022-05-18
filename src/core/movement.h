#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <array>
#include <utility>

#include "board.h"
#include "math.h"

class movement {
  board::t row_mask[4];
  board::t col_mask[4];

  math::u32 *score_table;

  board::t *up_table;
  board::t *down_table;
  board::t *left_table;
  board::t *right_table;

 public:
  using move_t = std::pair<board::t, math::u32>;

  movement() {
    row_mask[0] = 0xffff000000000000ull;
    row_mask[1] = 0x0000ffff00000000ull;
    row_mask[2] = 0x00000000ffff0000ull;
    row_mask[3] = 0x000000000000ffffull;

    col_mask[0] = 0xf000f000f000f000ull;
    col_mask[1] = 0x0f000f000f000f00ull;
    col_mask[2] = 0x00f000f000f000f0ull;
    col_mask[3] = 0x000f000f000f000full;

    math::u16 base_table[65536];
    math::u16 base_rev_table[65536];

    score_table = new math::u32[65536];

    up_table = new board::t[262144];
    down_table = new board::t[262144];
    left_table = new board::t[262144];
    right_table = new board::t[262144];

    for (int row = 0; row < 65536; row++) {
      score_table[row] = 0;
      int line[4] = {
        (row >> 0) & 0xf, (row >> 4) & 0xf, (row >> 8) & 0xf,
        (row >> 12) & 0xf};
      int furthest = 3;
      bool merged = false;

      for (int i = 3; i >= 0; i--) {
        if (!line[i]) continue;
        if (!merged && furthest < 3 && line[i] == line[furthest + 1]) {
          line[furthest + 1] = (line[furthest + 1] + 1) & 0xf;
          score_table[row] += (1u << line[furthest + 1]);
          line[i] = 0;
          merged = true;
        } else if (furthest == i)
          furthest--;
        else {
          line[furthest--] = line[i];
          line[i] = 0;
          merged = false;
        }
      }
      base_table[row] =
        line[0] | (line[1] << 4) | (line[2] << 8) | (line[3] << 12);

      base_rev_table[board::reverse_row(row)] =
        board::reverse_row(base_table[row]);
    }

    for (int row = 0; row < 65536; row++) {
      for (int i = 0; i < 4; i++) {
        left_table[row << 2 | i] = math::pdep64(base_table[row], row_mask[i]);
        right_table[row << 2 | i] =
          math::pdep64(base_rev_table[row], row_mask[i]);

        up_table[row << 2 | i] = math::pdep64(base_table[row], col_mask[i]);
        down_table[row << 2 | i] =
          math::pdep64(base_rev_table[row], col_mask[i]);
      }
    }
  }

  ~movement() {
    delete[] score_table;
    delete[] up_table;
    delete[] down_table;
    delete[] left_table;
    delete[] right_table;
  }

  inline std::array<move_t, 4> operator()(board::t b) {
    move_t up = {0, 0};
    move_t down = {0, 0};
    move_t left = {0, 0};
    move_t right = {0, 0};

    for (int i = 0; i < 4; i++) {
      board::t row = math::pext64(b, row_mask[i]);
      board::t col = math::pext64(b, col_mask[i]);

      up.first |= up_table[col << 2 | i];
      down.first |= down_table[col << 2 | i];
      left.first |= left_table[row << 2 | i];
      right.first |= right_table[row << 2 | i];

      left.second += score_table[row];
      up.second += score_table[col];
    }
    right.second = left.second;
    down.second = up.second;

    return {up, right, down, left};
  }
};

#endif
