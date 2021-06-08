#ifndef SEED_H
#define SEED_H
#include <chrono>

unsigned RandomSeed() {
    unsigned long long time = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    unsigned long long h = 14695981039346656037ULL;
    while (time) {
        h ^= time & 0xff;
        h *= 1099511628211ULL;
        time >>= 8;
    }
    return (unsigned)h ^ (unsigned)(h >> 32);
}

#endif