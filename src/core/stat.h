#ifndef STAT_H
#define STAT_H

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>

#include "board.h"
#include "math.h"

class stat {
  using ns = std::chrono::nanoseconds;
  using clock = std::chrono::system_clock;

  clock::time_point start;

  board::t best = 0;

  float games = 0;
  float moves = 0;
  math::u32 max = 0;
  float sum = 0;

  math::u32 rates[16] = {0};

  void header() {
    auto end = clock::now();
    auto duration = std::chrono::duration_cast<ns>(end - start);
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "avg: " << std::setw(8) << math::u32(sum / games) << "    ";
    std::cout << "max: " << std::setw(8) << max << '\n';
    std::cout << "m/s: " << std::setw(8)
              << math::u32(moves * 1e9 / float(duration.count())) << "    ";
    std::cout << "elapsed: " << math::u32(float(duration.count()) / 1e9)
              << "s\n";
  }

 public:
  stat() : start(clock::now()) {}

  void add(board::t b, float score, float move) {
    games += 1;
    sum += score;
    if (score > max) {
      max = score;
      best = b;
    }
    rates[board::max_tile(b)]++;
    moves += move;
  }

  void summary() {
    header();
    auto max_tile = 0;
    for (auto i = 15; i > 0; i--) {
      if (rates[i]) {
        max_tile = i;
        break;
      }
    }
    std::cout << std::setw(7) << (1 << max_tile);
    std::cout << std::setw(8) << float(rates[max_tile]) * 100.0f / games
              << "%\n\n";
  }

  void print() {
    header();
    auto accu = 0.0f;
    for (auto i = 15; i > 0; i--) {
      if (!rates[i]) continue;
      accu += rates[i];
      std::cout << std::setw(7) << (1 << i);
      std::cout << std::setw(8) << accu * 100.0f / games << "%\n";
    }
    std::cout << '\n';
    std::cout << "best game: (" << max << ")\n";
    board::print(best);
  }

  void join(const stat& other) {
    start = std::min(start, other.start);
    games += other.games;
    moves += other.moves;
    sum += other.sum;
    if (max < other.max) {
      max = other.max;
      best = other.best;
    }
    for (auto i = 0; i < 16; i++) rates[i] += other.rates[i];
  }

  void clear() {
    start = clock::now();
    games = 0;
    moves = 0;
    max = 0;
    sum = 0;
    best = 0;
    for (auto i = 0; i < 16; i++) rates[i] = 0;
  }
};

#endif
