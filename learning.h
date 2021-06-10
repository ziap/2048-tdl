#ifndef LEARNING_H
#define LEARNING_H

#include <numeric>
#include <queue>
#include <utility>
#include <unordered_map>

#define USE_COHERENCE
#include "tuplenet.h"

std::ofstream score_log("score.tcl.csv");

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
    unsigned stage = 0;

    std::queue<std::pair<board_t, int>> starts;

    T network;

    std::vector<State> path;

    float rate = 0.1f;
    Learning(unsigned i = 0, float a = 1.0f, float l = 0.0f, int u = 1000) {
        rate = a / (8 * network.length);
        lambda = l;
        interval = u;
        stage = i;
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

    void BackwardLearning() {
        float exact = 0, error = 0;
        for (path.pop_back();path.size(); path.pop_back()) {
            State move = path.back();
            error = error * lambda + exact - (move.value - move.reward);
            exact = move.reward + network.Update(move.after, rate * error);
        }
    }
    
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
            std::cout << "STAGE " << stage + 1;
            std::cout << '\t' << n << "\tmean = " << mean;
            std::cout << "\tmax = " << max;
            std::cout << '\n';
            float accu = 0.0f;
            for (int i = 0xf; i > 0; i--) {
                if (stat[i]) {
                    accu += float(stat[i]);
                    std::cout << '\t' << (1 << i) << '\t' << accu * 0.1f << "%\t" << float(stat[i]) * 0.1f << "%\n";
                }
            }
            score_log << mean << ',' << max << '\n';
            scores.clear();
            max_tile.clear();
        }
    }

    unsigned LearnEpisode(int n) {
        unsigned moves = 0;
        int score = 0;
        board_t board;
        if (!stage) board = AddTile(AddTile(0));
        else {
            board = starts.back().first;
            score = starts.back().second;
            starts.push({board, score});
        }
        for (;;) {
            State best = SelectBestMove(board);
            path.push_back(best);
            if (best.isValid()) {
                score += best.reward;
                board = AddTile(best.after);
                moves++;
            }
            else break;
        }
        BackwardLearning();
        MakeStat(n, board, score);
        return moves;
    }
    
    std::pair<board_t, int> CollectBoard() {
        board_t new_board = 0;
        int new_score = 0;
        int score = 0;
        board_t board;
        if (!stage) board = AddTile(AddTile(0));
        else {
            board = starts.back().first;
            score = starts.back().second;
            starts.push({board, score});
        }
        for (;;) {
            if (Sum(board) >= 2048 * (stage + 1)) {
                new_board = board;
                new_score = score;
                break;
            }
            State best = SelectBestMove(board);
            if (best.isValid()) {
                score += best.reward;
                board = AddTile(best.after);
            }
            else break;
        }
        return {new_board, new_score};
    }
};

#endif