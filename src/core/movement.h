#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <utility>
#include <cstring>
#include <array>

#include "board.h"
#include "math.h"

class movement {
  u32 score_table[65536];

  u16 forward_table[65536];
  u16 reverse_table[65536];

  enum dirs {
    UP,
    RIGHT,
    DOWN,
    LEFT
  };
 public:
  struct move_t {
    u64 board;
    u32 reward;
  };

  movement() {
    for (auto row = 0; row < 65536; row++) {
      score_table[row] = 0;

      std::array line = {
        (row >>  0) & 0xf,
        (row >>  4) & 0xf,
        (row >>  8) & 0xf,
        (row >> 12) & 0xf
      };

      auto furthest = 3;
      auto merged = false;

      for (auto i = 3; i >= 0; i--) {
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
      forward_table[row] =
        line[0] | (line[1] << 4) | (line[2] << 8) | (line[3] << 12);

      reverse_table[board::reverse_row(row)] =
        board::reverse_row(forward_table[row]);
    }
  }

  inline void operator()(board::t b, move_t results[4]) {
    const auto t = board::transpose(b);

    const std::array rows = {
      (b >> 48) & 0xffff,
      (b >> 32) & 0xffff,
      (b >> 16) & 0xffff,
      (b >>  0) & 0xffff
    };

    const std::array cols = {
      (t >> 48) & 0xffff,
      (t >> 32) & 0xffff,
      (t >> 16) & 0xffff,
      (t >>  0) & 0xffff,
    };

    results[dirs::UP].reward = 0;
    results[dirs::RIGHT].reward = 0;

    for (auto i = 0; i < 4; i++) {
      results[dirs::UP].board <<= 16;
      results[dirs::DOWN].board <<= 16;
      results[dirs::LEFT].board <<= 16;
      results[dirs::RIGHT].board <<= 16;

      results[dirs::UP].board |= forward_table[cols[i]];
      results[dirs::DOWN].board |= reverse_table[cols[i]];
      results[dirs::LEFT].board |= forward_table[rows[i]];
      results[dirs::RIGHT].board |= reverse_table[rows[i]];

      results[dirs::UP].reward += score_table[cols[i]];
      results[dirs::RIGHT].reward += score_table[rows[i]];
    }

    results[dirs::DOWN].reward = results[dirs::UP].reward;
    results[dirs::LEFT].reward = results[dirs::RIGHT].reward;

    results[dirs::UP].board = board::transpose(results[dirs::UP].board);
    results[dirs::DOWN].board = board::transpose(results[dirs::DOWN].board);
  }
};

#endif
