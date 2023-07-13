#ifndef EXPECTIMAX_H
#define EXPECTIMAX_H

#include <algorithm>

#include "../core/board.h"
#include "../core/math.h"
#include "../core/movement.h"
#include "transposition.h"

template <class Model>
class search {
  transposition<0x40000> cache;
  movement move;

  float move_node(board::t b, u32 depth) {
    auto max = 0.0f;

    movement::move_t moves[4];
    move(b, moves);

    for (const auto &child : moves)
      if (child.board != b)
        max = std::max(max, child.reward + spawn_node(child.board, depth));

    return max;
  }

  float spawn_node(board::t b, u32 depth) {
    auto expect = 0.0f;
    if (depth == 0) return Model::estimate(b);
    if (cache.lookup(b, depth, expect)) return expect;

    expect = 0.0f;
    auto mask = board::empty_pos(b);
    const auto empty = math::popcnt(mask);

    while (mask) {
      auto tile = mask & -mask;
      expect += move_node(b | tile, depth - 1) * 0.9;
      expect += move_node(b | (tile << 1), depth - 1) * 0.1;
      mask ^= tile;
    }

    return cache.update(b, depth, expect / empty);
  }

 public:
  u32 search_depth = 0;

  search(u32 d) : search_depth(d) {}

  movement::move_t suggest_move(board::t b) {
    movement::move_t result = { b, 0 };
    auto max = 0.0f;

    movement::move_t moves[4];
    move(b, moves);

    for (const auto& child : moves) {
      if (child.board != b) {
        const auto val = spawn_node(child.board, search_depth) + child.reward;
        if (result.board == b || max < val) {
          max = val;
          result = child;
        }
      }
    }

    return result;
  }

  int suggest_dir(board::t b) {
    auto result = -1;
    auto max = 0.0f;
    
    movement::move_t moves[4];
    move(b, moves);

    for (auto i = 0; i < 4; ++i) {
      if (moves[i].board != b) {
        auto val = spawn_node(moves[i].board, search_depth) + moves[i].reward;
        if (result == -1 || max < val) {
          max = val;
          result = i;
        }
      }
    }

    return result;
  }
};

#endif
