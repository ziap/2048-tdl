#include "learning.h"

#include "seed.h"
#include "getkey.h"

Learning<nw5x4tuples> tdl[16];//(0, 1.0, 0.5, 1000);

int main() {
    long long seed = RandomSeed();
    srand(seed);
    for (int i = 0; i < 16; i++) {
        tdl[i] = Learning<nw5x4tuples>(i, 1.0, 0.5, 1000);
        tdl[i].network.Load(std::string("weights/stage") + std::to_string(i + 1) + ".bin");
    }

    std::cout << "seed = " << seed << "\t learning rate = " << tdl[0].rate << '\n';
    unsigned long long moves = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 16; i++) {
        if (i) std::copy(std::begin(tdl[i - 1].network.weights), std::end(tdl[i - 1].network.weights), std::begin(tdl[i].network.weights));
        for (int n = 1; n <= 1000000; n++) moves += tdl[i].LearnEpisode(n);
        if (i < 15) while (tdl[i + 1].starts.size() < 20000) {
            std::pair<board_t, int> new_state = tdl[i].CollectBoard();
            if (new_state.first) tdl[i + 1].starts.push(new_state);
            std::cout << "\rCollecting boards " << tdl[i + 1].starts.size() << '/' << 20000;
        }
        std::cout << '\n';
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    auto duration_h = std::chrono::duration_cast<std::chrono::hours>(end - start);
    std::cout << "training done after " << duration_h.count() << " hours\n";
    std::cout << "speed = " << (float)moves * 1e9 / (float)duration.count() << " moves per second\n";
    
    for (int i = 0; i < 16; i++) tdl[i].network.Save(std::string("weights/stage") + std::to_string(i + 1) + ".bin");
}