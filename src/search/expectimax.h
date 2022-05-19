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

  float move_node(board::t b, math::u32 depth) {
    auto max = 0.0f;

    for (const auto& child : move(b))
      if (child.first != b)
        max = std::max(max, child.second + spawn_node(child.first, depth));

    return max;
  }

  float spawn_node(board::t b, math::u32 depth) {
    auto expect = 0.0f;
    if (depth == 0) return Model::estimate(b);
    if (cache.lookup(b, depth, &expect)) return expect;

    expect = 0.0f;
    auto mask = board::empty_pos(b);
    auto empty = float((mask * 0x1111111111111111ull) >> 60);

    while (mask) {
      auto tile = mask & (~mask + 1);
      expect += move_node(b | tile, depth - 1) * 0.9;
      expect += move_node(b | (tile << 1), depth - 1) * 0.1;
      mask ^= tile;
    }

    return cache.update(b, depth, expect / empty);
  }

 public:
  math::u32 search_depth = 0;

  search(math::u32 d) : search_depth(d) {}

  movement::move_t suggest_move(board::t b) {
    movement::move_t result = {b, 0};
    auto max = 0.0f;

    for (const auto& child : move(b)) {
      if (child.first != b) {
        auto val = spawn_node(child.first, search_depth) + child.second;
        if (result.first == b || max < val) {
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

    auto children = move(b);

    for (int i = 0; i < 4; i++) {
      const auto& child = children[i];
      if (child.first != b) {
        auto val = spawn_node(child.first, search_depth) + child.second;
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
