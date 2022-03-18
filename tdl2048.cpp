#include "learning.h"
#include "seed.h"

#include <getopt.h>
#include <cstdlib>

#ifndef STRUCTURE
    #define STRUCTURE nw4x6
#endif

#ifndef FILE_NAME
    #define FILE_NAME "weights.bin"
#endif

int main(int argc, char* argv[]) {
    int c;
    float alpha = 1.0;
    float lambda = 0.5;
    unsigned games = 100000;
    bool read = false;
    bool write = false;
    while ((c = getopt(argc, argv, "a:l:i:rw")) != -1) switch (c) {
    case 'a':
        alpha = atof(optarg);
        break;
    case 'l':
        lambda = atof(optarg);
        break;
    case 'i':
        games = atoi(optarg) * 100000;
        break;
    case 'r':
        read = true;
        break;
    case 'w':
        write = true;
        break;
    }
    Learning<STRUCTURE> tdl(alpha, lambda, 1000);
    long long seed = RandomSeed();
    srand(seed);

    if (read) tdl.Load(FILE_NAME);
    else std::fill(tdl.weights, tdl.weights + tdl.weights_len, 160.0f);

    std::cout << "Number of weights: " << tdl.weights_len << '\n';
    std::cout << "seed = " << seed << "\t learning rate = " << alpha << '\n';
    unsigned long long moves = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (int n = 1; n <= games; n++) moves += tdl.LearnEpisode(n);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    auto duration_h = std::chrono::duration_cast<std::chrono::hours>(end - start);
    std::cout << "training done after " << duration_h.count() << " hours\n";
    std::cout << "speed = " << (float)moves * 1e9 / (float)duration.count() << " moves per second\n";
    
    if (write) tdl.Save(FILE_NAME);
}
