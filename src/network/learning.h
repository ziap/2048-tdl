#ifndef LEARNING_H
#define LEARNING_H

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "../core/board.h"
#include "../core/math.h"
#include "../core/movement.h"
#include "../core/stat.h"
#include "tuplenet.h"

template <class model>
class learning {
  float alpha;
  float lambda;
  bool restart;
  movement move;

 public:
  learning(float a, float l, bool r)
    : alpha(a / (8 * model::length)), lambda(l), restart(r){};

  movement::move_t suggest_move(board::t b) {
    movement::move_t result = {b, 0};
    auto max = 0.0f;

    for (auto child : move(b)) {
      if (child.first != b) {
        auto val = model::estimate(child.first) + child.second;
        if (result.first == b || max < val) {
          max = val;
          result = child;
        }
      }
    }

    return result;
  }

  stat learn_episodes(math::u32 n, unsigned trd_id) {
    auto digits = std::to_string(n).length();

    stat result;
    math::random rng;

    for (auto i = 1u; i <= n; i++) {
      auto initboard = board::add_tile(board::add_tile(0, rng), rng);
      auto update_stat = true;
      auto moves = 0u;

      while (initboard) {
        auto b = initboard;
        initboard = 0;
        std::vector<movement::move_t> path;
        auto score = 0;

        for (;;) {
          auto best = suggest_move(b);
          if (best.first != b) {
            path.push_back(best);
            score += best.second;
            b = board::add_tile(best.first, rng);
            moves++;
          } else
            break;
        }

        if (restart && path.size() > 10)
          initboard = board::add_tile(path[path.size() / 2].first, rng);
        
        auto exact = 0.0f;
        auto error = 0.0f;
        for (; path.size(); path.pop_back()) {
          auto action = path.back();
          error = exact - model::estimate(action.first);
          exact = action.second + lambda * exact +
                  (1 - lambda) * model::update(action.first, alpha * error);
        }

        if (update_stat) {
          result.add(b, score, moves);
          update_stat = false;
        }
      }

      if (i % 1000 == 0) {
        std::cout << "progress: " << std::setw(digits) << i << '/' << n
                  << "    thread: #" << trd_id << '\n';
        result.summary();
        if (i < n) result.clear();
      }
    }

    return result;
  }
};

#endif