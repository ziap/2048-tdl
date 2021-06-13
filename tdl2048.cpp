#include "learning.h"

#include "seed.h"

#ifndef STRUCTURE
    #define STRUCTURE nw4x6
#endif

Learning<STRUCTURE> tdl(0, 1.0, 0.5, 1000);

int main() {
    long long seed = RandomSeed();
    srand(seed);
    tdl.network.Load("weights.bin");

    std::cout << "seed = " << seed << "\t learning rate = " << tdl.rate << '\n';
    unsigned long long moves = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (int n = 1; n <= 100000; n++) moves += tdl.LearnEpisode(n);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    auto duration_h = std::chrono::duration_cast<std::chrono::hours>(end - start);
    std::cout << "training done after " << duration_h.count() << " hours\n";
    std::cout << "speed = " << (float)moves * 1e9 / (float)duration.count() << " moves per second\n";
    
    tdl.network.Save("weights.bin");
}
