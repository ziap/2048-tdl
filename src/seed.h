#ifndef SEED_H
#define SEED_H
#include <chrono>

// Generate a random seed
unsigned RandomSeed() {
    auto time = 0ull + std::chrono::high_resolution_clock::now().time_since_epoch().count();
    auto h = 14695981039346656037ULL;
    while (time) {
        h ^= time & 0xff;
        h *= 1099511628211ULL;
        time >>= 8;
    }
    return (unsigned)h ^ (unsigned)(h >> 32);
}

#endif
