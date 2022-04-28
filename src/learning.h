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
#include "tuplenet.h"

template <class T>
class Learning : public T {
   private:
    float lambda = 0.0f;

    unsigned interval = 1000;

    bool restart = false;

    std::ofstream out;

    std::vector<int> scores, max_tile;

   public:
    std::queue<std::pair<board_t, int>> starts;

    float rate = 0.1f;
    Learning(float a = 1.0f, float l = 0.0f, int u = 1000, bool r = false, bool h = false) {
        rate = a / (8 * this->length);
        lambda = l;
        interval = u;
        restart = r;
        if (h) {
            out.open("history.csv");
            out << "game,mean,max,32768,16384,8192,4096,2048\n";
            out << "0,0,0,0%,0%,0%,0%,0%\n";
        }
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

    // Print statistics of the learning process
    void MakeStat(int n, board_t b, int score) {
        scores.push_back(score);
        max_tile.push_back(MaxRank(b));
        if (n % interval == 0) {
            auto sum = 0;
            auto max = 0;
            for (int i : scores) {
                sum += i;
                max = std::max(max, i);
            }
            auto stat = std::array<int, 16>{};
            for (int i : max_tile) stat[i]++;
            auto mean = float(sum) / float(interval);
            std::cout << n << "\tmean = " << mean;
            std::cout << "\tmax = " << max;
            if (out.is_open()) out << n << "," << mean << "," << max;
            std::cout << '\n';
            auto accu = 0.0f;
            for (int i = 0xf; i > 0; i--) {
                if (stat[i]) {
                    accu += float(stat[i]);
                    std::cout << '\t' << (1 << i) << '\t' << accu * 0.1f << "%\t" << float(stat[i]) * 0.1f << "%\n";
                    if (out.is_open() && i > 10) out << "," << accu * 0.1f << "%";
                } else {
                    if (out.is_open() && i > 10) out << ",0%";
                }
            }
            scores.clear();
            max_tile.clear();
            if (out.is_open()) out << '\n';
        }
    }

    // Play a game and update the network
    unsigned LearnEpisode(int n) {
        auto moves = 0u;
        auto initboard = AddTile(AddTile(0));
        auto print_stats = true;
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
                MakeStat(n, board, score);
                print_stats = false;
            }
        }
        return moves;
    }
};

#endif
