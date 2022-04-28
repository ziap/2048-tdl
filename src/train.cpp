#include <getopt.h>

#include <cstdlib>

#include "learning.h"
#include "seed.h"

#ifndef STRUCTURE
#define STRUCTURE nw4x6
#endif

#ifndef FILE_NAME
#define FILE_NAME "weights.bin"
#endif

int main(int argc, char* argv[]) {
    auto c = -1;
    auto alpha = 1.0f;
    auto lambda = 0.5f;
    auto games = 1000u;
    auto read = false;
    auto write = false;
    auto restart = false;
    auto history = false;
    while ((c = getopt(argc, argv, "a:l:e:iorh")) != -1) switch (c) {
            case 'a': alpha = atof(optarg); break;
            case 'l': lambda = atof(optarg); break;
            case 'e': games = atoi(optarg) * 1000; break;
            case 'i': read = true; break;
            case 'o': write = true; break;
            case 'r': restart = true; break;
            case 'h': history = true; break;
        }
    Learning<STRUCTURE> tdl(alpha, lambda, 1000, restart, history);
    auto seed = RandomSeed();
    srand(seed);

    if (read) tdl.Load(FILE_NAME);

    std::cout << "Number of weights: " << tdl.weights_len << '\n';
    std::cout << "seed = " << seed << "\t learning rate = " << alpha << '\n';
    auto moves = 0ull;
    auto start = std::chrono::high_resolution_clock::now();
    for (auto n = 1; n <= games; n++) moves += tdl.LearnEpisode(n);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    auto duration_h = std::chrono::duration_cast<std::chrono::hours>(end - start);
    std::cout << "training done after " << duration_h.count() << " hours\n";
    std::cout << "speed = " << (float)moves * 1e9 / (float)duration.count() << " moves per second\n";

    if (write) tdl.Save(FILE_NAME);
}
