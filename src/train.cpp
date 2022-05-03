#include <getopt.h>

#include <cstdlib>
#include <thread>
#include <vector>
#include <future>

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
    auto thread_count = 1u;
    while ((c = getopt(argc, argv, "a:l:e:iort:")) != -1) switch (c) {
            case 'a': alpha = atof(optarg); break;
            case 'l': lambda = atof(optarg); break;
            case 'e': games = atoi(optarg) * 1000; break;
            case 'i': read = true; break;
            case 'o': write = true; break;
            case 'r': restart = true; break;
            case 't': thread_count = atoi(optarg); break;
        }
    Learning<STRUCTURE> tdl(alpha, lambda, restart);
    auto seed = RandomSeed();
    srand(seed);

    if (read) tdl.Load(FILE_NAME);

    std::cout << "Number of weights: " << tdl.weights_len << '\n';
    std::cout << "seed = " << seed << "\t learning rate = " << alpha << '\n';

    std::vector<std::future<Stat>> futures;

    auto LearnFunctor = [&tdl](unsigned n, unsigned thread) -> Stat { return tdl.LearnEpisode(n, thread); };

    for (auto i = 0; i < thread_count - 1; i++) futures.push_back(std::async(std::launch::async, LearnFunctor, games / thread_count, i));

    auto result = tdl.LearnEpisode(games - games / thread_count * (thread_count - 1), thread_count - 1);

    for (auto& f : futures) { result = result.Join(f.get()); }

    result.Print();
}
