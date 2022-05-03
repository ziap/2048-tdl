#ifndef STAT_H
#define STAT_H

#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>

#include "board.h"

class Stat {
    std::chrono::system_clock::time_point start;

    board_t best = 0;

    unsigned games = 0;
    float moves = 0;
    float max = 0;
    float sum = 0;
    unsigned rates[16] = {0};

    void Header() {
        auto end = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "average score: " << sum / (float)games << '\n';
        std::cout << "maximum score: " << max << '\n';
        std::cout << "speed: " << moves * 1e9 / (float)duration.count() << " moves per second\n";
    }

   public:
    Stat() : start(std::chrono::system_clock::now()) {}

    void Add(board_t b, float score, float move) {
        games++;
        sum += score;
        if (score > max) {
            max = score;
            best = b;
        }
        rates[MaxRank(b)]++;
        moves += move;
    }

    void Summary() {
        Header();
        auto max = 0;
        for (auto i = 15; i > 0; i--) {
            if (rates[i]) {
                max = i;
                break;
            }
        }
        std::cout << "max tile: " << (1 << max) << " \t" << rates[max] * 100.f / games << "%\n";
    }

    void Print() {
        Header();
        auto accu = 0;
        for (auto i = 15; i > 0; i--) {
            if (!rates[i]) continue;
            accu += rates[i];
            std::cout << (1 << i) << " \t" << accu << " \t" << accu * 100.f / float(games) << "%\n";
        }
        std::cout << "\nBest game:\n";
        PrintBoard(best);
    }

    Stat Join(Stat other) {
        Stat result;
        result.start = std::min(start, other.start);
        result.games = games + other.games;
        result.moves = moves + other.moves;
        if (max < other.max) {
            result.max = other.max;
            result.best = other.best;
        }
        else {
            result.max = max;
            result.best = best;
        }
        result.sum = sum + other.sum;
        for (auto i = 0; i < 16; i++) result.rates[i] = rates[i] + other.rates[i];
        return result;
    }

    void Clear() {
        start = std::chrono::system_clock::now();
        games = 0;
        moves = 0;
        max = 0;
        sum = 0;
        best = 0;
        for (auto i = 0; i < 16; i++) rates[i] = 0;
    }
};

#endif