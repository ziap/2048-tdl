#include "learning.h"

#include "seed.h"
#include "getkey.h"

Learning<nw5x4tuples> tdl(1.0, 0.5, 1000);
unsigned long long MAX_ACTIONS = 10000000000ULL;

int main() {
    DisableCanonical();
    long long seed = RandomSeed();
    srand(seed);
    tdl.network.Load("weights.bin");
    std::cout << "seed = " << seed << "\t learning rate = " << tdl.rate << '\n';
    auto start = std::chrono::high_resolution_clock::now();
    unsigned long long moves = 0;
    for (int n = 1; moves < MAX_ACTIONS; n++) {
        if (KeyPressed('x')) break;
        board_t board = AddTile(AddTile(0));
        int score = 0;
        for (;;) {
            State best = tdl.SelectBestMove(board);
            tdl.path.push_back(best);
            if (best.isValid()) {
                score += best.reward;
                board = AddTile(best.after);
                moves++;
            }
            else break;
        }
        tdl.BackwardLearning();
        tdl.MakeStat(n, board, score, (double)moves / float(MAX_ACTIONS));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    auto duration_h = std::chrono::duration_cast<std::chrono::hours>(end - start);
    std::cout << "training done after " << duration_h.count() << " hours\n";
    std::cout << "speed = " << (float)moves * 1e9 / (float)duration.count() << " moves per second\n";
    tdl.network.Save("weights.bin");
    EnableCanonical();
}