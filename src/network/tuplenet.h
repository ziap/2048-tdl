#ifndef TUPLENET_H
#define TUPLENET_H

#include <fstream>

#include "pattern.h"

template <bool tc, class... Features>
class network {
  template <class... Targs>
  struct tuples;

  template <class T>
  struct tuples<T> {
    static constexpr math::u64 number_of_weights = 1 << (4 * T::length);

    static float estimate(board::t b, float *w) { return T::estimate(b, w); }

    static float update(board::t b, float *w, float u) {
      return T::update(b, w, u, tuples<Features...>::number_of_weights);
    }
  };

  template <class T, class... Targs>
  struct tuples<T, Targs...> {
    static constexpr math::u64 number_of_weights =
      tuples<T>::number_of_weights + tuples<Targs...>::number_of_weights;

    static float estimate(board::t b, float *w) {
      return tuples<T>::estimate(b, w) +
             tuples<Targs...>::estimate(b, w + tuples<T>::number_of_weights);
    }

    static float update(board::t b, float *w, float u) {
      return tuples<T>::update(b, w, u) +
             tuples<Targs...>::update(b, w + tuples<T>::number_of_weights, u);
    }
  };

 public:
  static constexpr math::u32 length = sizeof...(Features);

  static constexpr math::u64 weight_len =
    tuples<Features...>::number_of_weights;

  
  
  static float *get_weights() {
    if constexpr (tc) {
      static float *weights = new float[weight_len * 3];
      return weights;
    } else {
      static float *weights = new float[weight_len];
      return weights;
    }
  }

  static float estimate(board::t b) {
    return tuples<Features...>::estimate(b, get_weights());
  }

  static float update(board::t b, float u) {
    return tuples<Features...>::update(b, get_weights(), u);
  }

  static void load(std::string path) {
    std::ifstream fin(path.c_str(), std::ios::out | std::ios::binary);
    auto weights = get_weights();
    if (fin.is_open()) fin.read((char *)weights, sizeof(float) * weight_len);
    fin.close();
    if constexpr (tc) {
      fin.open((path + ".tc").c_str(), std::ios::out | std::ios::binary);
      if (fin.is_open())
        fin.read(
          (char *)(weights + weight_len), sizeof(float) * weight_len * 2);
      fin.close();
    }
  }

  static void save(std::string path) {
    std::ofstream fout(path.c_str(), std::ios::out | std::ios::binary);
    auto weights = get_weights();
    fout.write((char *)weights, sizeof(float) * weight_len);
    fout.close();
    if constexpr (tc) {
      fout.open((path + ".tc").c_str(), std::ios::out | std::ios::binary);
      fout.write(
        (char *)(weights + weight_len), sizeof(float) * weight_len * 2);
      fout.close();
    }
  }
};

template <math::u64... pattern>
struct tuple_network
  : public network<false, typename extract_pattern<false, pattern>::value...> {
  using with_coherence =
    network<true, typename extract_pattern<true, pattern>::value...>;
};

using nw5x4 = tuple_network<0x3210, 0x7654, 0x4510, 0x6521, 0xa965>;
using nw4x5 = tuple_network<0x43210, 0x87654, 0x54210, 0x98654>;
using nw6x5 =
  tuple_network<0x43210, 0x87654, 0xcba98, 0x54210, 0x98645, 0xdca98>;
using nw4x6 = tuple_network<0x543210, 0x987654, 0x654210, 0xA98654>;
using nw5x6 = tuple_network<0x543210, 0x987654, 0xdcba98, 0x654210, 0xa98654>;
using nw8x6 = tuple_network<
  0x654210, 0xd96521, 0x543210, 0xa76510, 0xa95210, 0xed9510, 0xd98510,
  0xa64210>;

#endif