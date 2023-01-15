#ifndef MATH_H
#define MATH_H

#include <chrono>
#include <random>

namespace math {
using u64 = unsigned long long;
using u32 = unsigned;
using u16 = unsigned short;

#if defined(__x86_64__) && defined(__BMI2__)
inline u64 pdep64(u64 val, u64 mask) {
  return __builtin_ia32_pdep_di(val, mask);
}

inline u64 pext64(u64 val, u64 mask) {
  return __builtin_ia32_pext_di(val, mask);
}
#else
#warning "PEXT64 and PDEP64 is slow without BMI2"

inline u64 pdep64(u64 val, u64 mask) {
  u64 res = 0;
  for (u64 bb = 1; mask; bb <<= 1) {
    if (val & bb) res |= mask & -mask;
    mask &= mask - 1;
  }
  return res;
}

inline u64 pext64(u64 val, u64 mask) {
  u64 res = 0;
  for (u64 bb = 1; mask; bb <<= 1) {
    if (val & mask & -mask) res |= bb;
    mask &= mask - 1;
  }
  return res;
}
#endif

inline u32 generate_seed() {
  u64 time =
    std::chrono::high_resolution_clock::now().time_since_epoch().count();
  u64 h = 14695981039346656037ULL;
  while (time) {
    h ^= time & 0xff;
    h *= 1099511628211ULL;
    time >>= 8;
  }
  return (u32)h & (u32)(h >> 32);
}

class random {
  std::mt19937_64 engine;

 public:
  random(u32 seed = generate_seed()) { engine.seed(seed); }

  u64 operator()() { return engine(); }
};

};  // namespace math

#endif
