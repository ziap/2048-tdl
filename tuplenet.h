#ifndef TUPLENET_H
#define TUPLENET_H

#include <array>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include "board.h"

// The pattern's indexer
template <int... Targs>
struct Indexer;

template <int T>
struct Indexer<T> {
    static constexpr board_t mask = 0xFULL << (4 * (15 - T));
};

template <int T, int... Targs>
struct Indexer<T, Targs...> {
    static constexpr board_t mask = (Indexer<T>::mask | Indexer<Targs...>::mask);
};

template <int... pattern>
class Pattern {
   private:
   public:
    // Length of the pattern
    static constexpr int length = sizeof...(pattern);

   private:
    static constexpr int patt[length] = {pattern...};

   public:
    static float Estimate(board_t b, float* w) {
        float value = 0;
        for (int i = 0; i < 8; i++) {
            value += w[__builtin_ia32_pext_di(b, Indexer<pattern...>::mask)];
            if (i & 1) b = Flip(b);
            else
                b = Transpose(b);
        }
        return value;
    }

    static float Update(board_t b, float u, float* w, board_t n) {
        float value = 0;
        for (int i = 0; i < 8; i++) {
            board_t index = __builtin_ia32_pext_di(b, Indexer<pattern...>::mask);
            float alpha = 1;
#ifdef USE_COHERENCE
            float& error = w[index + n];
            float& abs_error = w[index + 2 * n];
            if (abs_error != 0) alpha = std::abs(error) / abs_error;
#endif
            w[index] += alpha * u;
            value += w[index];
#ifdef USE_COHERENCE
            error += u;
            abs_error += std::abs(u);
#endif
            if (i & 1) b = Flip(b);
            else
                b = Transpose(b);
        }
        return value;
    }
};

template <class... Features>
class TupleNetwork {
   private:
    template <class... Targs>
    struct Tuples;

    template <class T>
    struct Tuples<T> {
        static constexpr unsigned number_of_weights = (1 << (4 * T::length));

        static float Estimate(board_t b, float* w) { return T::Estimate(b, w); }

        static float Update(board_t b, float u, float* w) { return T::Update(b, u, w, Tuples<Features...>::number_of_weights); }
    };

    template <class T, class... Targs>
    struct Tuples<T, Targs...> {
        static constexpr unsigned number_of_weights = Tuples<T>::number_of_weights + Tuples<Targs...>::number_of_weights;

        static float Estimate(board_t b, float* w) { return Tuples<T>::Estimate(b, w) + Tuples<Targs...>::Estimate(b, w + Tuples<T>::number_of_weights); }

        static float Update(board_t b, float u, float* w) {
            return Tuples<T>::Update(b, u, w) + Tuples<Targs...>::Update(b, u, w + Tuples<T>::number_of_weights);
        }
    };

   public:
    // Size of the model
    static constexpr unsigned length = sizeof...(Features);

    static constexpr unsigned weights_len = Tuples<Features...>::number_of_weights;

    // The weights of the model
#ifndef USE_COHERENCE
    float* weights = new float[weights_len];
#else
    float* weights = new float[weights_len * 3];
#endif

    // Estimate the value of a board
    float Estimate(board_t b) { return Tuples<Features...>::Estimate(b, weights); }

    // Update the value of a board and return the updated value
    float Update(board_t b, float u) { return Tuples<Features...>::Update(b, u, weights); }

    // Save the model to a binary file
    void Save(std::string path) {
        std::ofstream fout(path.c_str(), std::ios::out | std::ios::binary);
        fout.write((char*)weights, sizeof(float) * weights_len);
        fout.close();
#ifdef USE_COHERENCE
        fout.open((path + ".tc").c_str(), std::ios::out | std::ios::binary);
        fout.write((char*)(weights + weights_len), sizeof(float) * weights_len * 2);
        fout.close();
#endif
    }

    // Load the model from a binary file
    void Load(std::string path) {
        std::ifstream fin(path.c_str(), std::ios::out | std::ios::binary);
        if (fin.is_open()) fin.read((char*)weights, sizeof(float) * weights_len);
        fin.close();
#ifdef USE_COHERENCE
        fin.open((path + ".tc").c_str(), std::ios::out | std::ios::binary);
        fin.read((char*)(weights + weights_len), sizeof(float) * weights_len * 2);
        fin.close();
#endif
    }
};

// Predefined structures

using nw4x6 = TupleNetwork<Pattern<0, 1, 2, 3, 4, 5>, Pattern<4, 5, 6, 7, 8, 9>, Pattern<0, 1, 2, 4, 5, 6>, Pattern<4, 5, 6, 8, 9, 10> >;

using nw5x6 =
    TupleNetwork<Pattern<0, 1, 2, 3, 4, 5>, Pattern<4, 5, 6, 7, 8, 9>, Pattern<8, 9, 10, 11, 12, 13>, Pattern<0, 1, 2, 4, 5, 6>, Pattern<4, 5, 6, 8, 9, 10> >;

using nw8x6 = TupleNetwork<Pattern<0, 1, 2, 4, 5, 6>, Pattern<1, 2, 5, 6, 9, 13>, Pattern<0, 1, 2, 3, 4, 5>, Pattern<0, 1, 5, 6, 7, 10>,
                           Pattern<0, 1, 2, 5, 9, 10>, Pattern<0, 1, 5, 9, 13, 14>, Pattern<0, 1, 5, 8, 9, 13>, Pattern<0, 1, 2, 4, 6, 10> >;

using nw5x4 = TupleNetwork<Pattern<0, 1, 2, 3>, Pattern<4, 5, 6, 7>, Pattern<0, 1, 4, 5>, Pattern<1, 2, 5, 6>, Pattern<5, 6, 9, 10> >;

using nw4x5 = TupleNetwork<Pattern<0, 1, 2, 3, 4>, Pattern<4, 5, 6, 7, 8>, Pattern<0, 1, 2, 4, 5>, Pattern<4, 5, 6, 8, 9> >;

using nw6x5 = TupleNetwork<Pattern<0, 1, 2, 3, 4>, Pattern<4, 5, 6, 7, 8>, Pattern<8, 9, 10, 11, 12>, Pattern<0, 1, 2, 4, 5>, Pattern<4, 5, 6, 8, 9>,
                           Pattern<8, 9, 10, 12, 13> >;

#endif
