#include "learning.h"

#include "seed.h"

#ifndef STRUCTURE
    #define STRUCTURE nw4x6
#endif

#ifndef FILE_NAME
    #define FILE_NAME "weights.bin"
#endif

Learning<STRUCTURE> tdl(0, 1.0, 0.5, 1000);

int main() {
    long long seed = RandomSeed();
    srand(seed);
    tdl.network.Load(FILE_NAME);

    std::cout << "seed = " << seed << "\t learning rate = " << tdl.rate << '\n';
    unsigned long long moves = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (int n = 1; n <= 100000; n++) moves += tdl.LearnEpisode(n);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    auto duration_h = std::chrono::duration_cast<std::chrono::hours>(end - start);
    std::cout << "training done after " << duration_h.count() << " hours\n";
    std::cout << "speed = " << (float)moves * 1e9 / (float)duration.count() << " moves per second\n";
    
    tdl.network.Save(FILE_NAME);
}
