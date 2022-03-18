#ifndef LEARNING_H
#define LEARNING_H

#include <numeric>
#include <queue>
#include <utility>
#include <unordered_map>

#ifndef ENABLE_TC
    #define ENABLE_TC true
#endif

#if ENABLE_TC
    #define USE_COHERENCE
#endif
#include "tuplenet.h"

template<class T>
class Learning : public T {
private:
    float lambda = 0.0f;

    unsigned interval = 1000;

    std::vector<int> scores, max_tile;

public:

    std::queue<std::pair<board_t, int>> starts;

    float rate = 0.1f;
    Learning(float a = 1.0f, float l = 0.0f, int u = 1000) {
        rate = a / (8 * this->length);
        lambda = l;
        interval = u;
    }

    std::pair<board_t, float> SelectBestMove(board_t b) {
        std::pair<board_t, float> best = {0, -1};
        float best_value = 0;
        for (int i = 0; i < 4; i++) {
            board_t after = move(b, i);
            if (after != b) {
                float reward = move.Score(b, i);
                float value = reward + this->Estimate(after);
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
            int sum = 0;
            int max = 0;
            for (int i : scores) {
                sum += i;
                max = std::max(max, i);
            }
            int stat[16] = { 0 };
            for (int i : max_tile) stat[i]++;
            float mean = float(sum) / float(interval);
            std::cout << n << "\tmean = " << mean;
            std::cout << "\tmax = " << max;
            std::cout << '\n';
            float accu = 0.0f;
            for (int i = 0xf; i > 0; i--) {
                if (stat[i]) {
                    accu += float(stat[i]);
                    std::cout << '\t' << (1 << i) << '\t' << accu * 0.1f << "%\t" << float(stat[i]) * 0.1f << "%\n";
                }
            }
            scores.clear();
            max_tile.clear();
        }
    }

    // Play a game and update the network
    unsigned LearnEpisode(int n) {
        unsigned moves = 0;
        board_t initboard = AddTile(AddTile(0));
        bool print_stats = true;
        while (initboard) {
            board_t board = initboard;
            initboard = 0;
            std::vector<std::pair<board_t, float>> path;
            int score = 0;
            for (;;) {
                std::pair<board_t, float> best = SelectBestMove(board);
                if (best.second != -1) {
                    path.push_back(best);
                    score += best.second;
                    board = AddTile(best.first);
                    moves++;
                }
                else break;
            }
            //if (path.size() > 10) initboard = AddTile(path[path.size() >> 1].first);
            float exact = 0, error = 0;
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
