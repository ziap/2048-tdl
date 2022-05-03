#ifndef LEARNING_H
#define LEARNING_H

#include <fstream>
#include <numeric>
#include <queue>
#include <unordered_map>
#include <utility>

#ifndef ENABLE_TC
#define ENABLE_TC true
#endif

#if ENABLE_TC
#define USE_COHERENCE
#endif

#include "stat.h"
#include "tuplenet.h"

template <class T>
class Learning : public T {
   private:
    float lambda = 0.0f;

    bool restart = false;

    std::vector<int> scores, max_tile;

   public:
    std::queue<std::pair<board_t, int>> starts;

    float rate = 0.1f;
    Learning(float a = 1.0f, float l = 0.0f, bool r = false) {
        rate = a / (8 * this->length);
        lambda = l;
        restart = r;
    }

    std::pair<board_t, float> SelectBestMove(board_t b) {
        auto best = std::make_pair(0ull, -1.0f);
        auto best_value = 0.0f;
        for (int i = 0; i < 4; i++) {
            auto after = move(b, i);
            if (after != b) {
                auto reward = (float)move.Score(b, i);
                auto value = reward + this->Estimate(after);
                if (best.second == -1 || value > best_value) {
                    best = {after, reward};
                    best_value = value;
                }
            }
        }
        return best;
    }

    // Play a game and update the network
    Stat LearnEpisode(unsigned n, unsigned thread) {
        Stat stat;

        for (int i = 1; i <= n; i++) {
            auto initboard = AddTile(AddTile(0));
            auto print_stats = true;
            auto moves = 0;
            while (initboard) {
                auto board = initboard;
                initboard = 0;
                std::vector<std::pair<board_t, float>> path;
                auto score = 0;
                for (;;) {
                    auto best = SelectBestMove(board);
                    if (best.second != -1) {
                        path.push_back(best);
                        score += best.second;
                        board = AddTile(best.first);
                        moves++;
                    } else
                        break;
                }
                if (restart && path.size() > 10) initboard = AddTile(path[path.size() >> 1].first);
                auto exact = 0.0f;
                auto error = 0.0f;
                for (; path.size(); path.pop_back()) {
                    std::pair<board_t, float> move = path.back();
                    error = exact - this->Estimate(move.first);
                    exact = move.second + lambda * exact + (1 - lambda) * this->Update(move.first, rate * error);
                }
                if (print_stats) {
                    stat.Add(board, score, moves);
                    print_stats = false;
                }
            }

            if (i % 1000 == 0) {
                std::cout << "progress: " << i << '/' << n << " \tthread: " << thread << '\n';
                stat.Summary();
                std::cout << '\n';
                if (i < n) stat.Clear();
            }
        }

        return stat;
    }
};

#endif
