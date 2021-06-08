#ifndef LEARNING_H
#define LEARNING_H

#include <numeric>
#include <unordered_map>
#include "tuplenet.h"

class State {
public:
    board_t before;
    board_t after;
    float reward;
    int action;
    float value = 0;
    State(board_t b, int dir) {
        action = dir;
        before = b;
        if (dir == -1) {
            after = b;
            reward = 0;
        }
        else {
            after = move(b, dir);
            reward = move.Score(b, dir);
        }
    }
    bool isValid() { return before != after; }
    void operator=(State s) {
        action = s.action;
        before = s.before;
        after = s.after;
        value = s.value;
        reward = s.reward;
    }
};

template<class T>
class Learning {
private:
    float lambda = 0.0f;

    unsigned interval = 1000;

    std::vector<int> scores, max_tile;

    T errors, abs_errors;

public:
    T network;

    std::vector<State> path;

    float rate = 0.1f;
    Learning(float a = 1.0f, float l = 0.0f, int u = 1000) {
        rate = a / (8 * network.length);
        lambda = l;
        interval = u;
    }

    State SelectBestMove(board_t b) {
        State best(b, -1);
        for (int i = 0; i < 4; i++) {
            State after(b, i);
            if (after.isValid()) {
                after.value = after.reward + network.Estimate(after.after);
                if (!best.isValid() || after.value > best.value) best = after;
            }
        }
        return best;
    }

    float TCUpdate(board_t b, float u) {
        float value = 0;
        std::vector<board_t> active_weights = network.GetIndex(b);
        for (board_t weight : active_weights) {
            float alpha = ((abs_errors.weights[weight] == 0.0f) ? 1.0f : (abs(errors.weights[weight]) / abs_errors.weights[weight]));
            network.weights[weight] += rate * alpha * u / (8 * network.length);
            value += network.weights[weight];
            errors.weights[weight] += u;
            abs_errors.weights[weight] += abs(u);
        }
        return value;
    }

    void BackwardLearning() {
        std::unordered_map<board_t, float> delta;
        float exact = 0;
        for (path.pop_back();path.size(); path.pop_back()) {
            State move = path.back();
            float error = exact - (move.value - move.reward);
            float curr_lambda = 1;
            for (std::vector<State>::reverse_iterator i = path.rbegin(); i != path.rend() && i != path.rbegin() + 5; i++) {
                delta[i->after] += curr_lambda * error; // Delayed update
                curr_lambda *= lambda;
            }
            exact = move.reward + TCUpdate(move.after, delta[move.after]);
            delta.erase(delta.find(move.after));
        }
    }
    
    void MakeStat(int n, board_t b, int score, double progress) {
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
            float mean = float(sum) / 1000;
            std::cout << n << "\tmean = " << mean;
            std::cout << "\tmax = " << max;
            std::cout << "\tprogress: " << progress * 100 << '%';
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
};

#endif