#ifndef PATTERN_H
#define PATTERN_H

#include <algorithm>
#include <type_traits>

#include "../core/board.h"
#include "../core/math.h"

template <bool tc, math::u64 n>
struct extract_pattern {
  template <math::u16... pattern>
  static constexpr bool ordered() {
    auto last = -1;
    for (int i : {pattern...}) {
      if (i <= last) return false;
      last = i;
    }
    return true;
  }

  template <math::u16... pattern>
  struct indexer;

  template <math::u16 T>
  struct indexer<T> {
    static constexpr board::t mask = 0xfull << (4 * (15 - T));
  };

  template <math::u16 T, math::u16... Ts>
  struct indexer<T, Ts...> {
    static constexpr board::t mask = indexer<T>::mask | indexer<Ts...>::mask;
  };

  template <math::u16... pattern>
  struct tuple {
    static constexpr math::u16 length = sizeof...(pattern);

   private:
    static constexpr board::t index(board::t b) {
      return math::pext64(b, indexer<pattern...>::mask);
    }

   public:
    static float estimate(board::t b, float* w) {
      auto value = 0.0f;
      for (auto i = 0; i < 8; i++) {
        value += w[index(b)];

        if (i & 1) b = board::flip(b);
        else
          b = board::transpose(b);
      }
      return value;
    }

    static float update(board::t b, float* w, float u, math::u64 l) {
      float value = 0;
      for (auto i = 0; i < 8; i++) {
        auto ind = index(b);
        auto alpha = 1.0f;
        if constexpr (tc) {
          auto& error = w[ind + l];
          auto& abs_error = w[ind + 2 * l];
          if (abs_error != 0) alpha = std::abs(error) / abs_error;
          error += u;
          abs_error += std::abs(u);
        }
        w[ind] += alpha * u;
        value += w[ind];

        if (i & 1) b = board::flip(b);
        else
          b = board::transpose(b);
      }
      return value;
    }
  };

  template <math::u64 x, math::u16... xs>
  struct extractor : extractor<(x >> 4), xs..., (x & 0xf)> {};

  template <math::u16... xs>
  struct extractor<0, xs...> {
    using value = typename std::enable_if<ordered<xs...>(), tuple<xs...>>::type;
  };

  using value = typename extractor<n>::value;
};

#endif