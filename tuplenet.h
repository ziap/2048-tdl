#ifndef TUPLENET_H
#define TUPLENET_H

#include <fstream>
#include <string>
#include <array>
#include <vector>
#include <iterator>
#include "board.h"

template <int...pattern>
class Pattern {
private:

    // Boards used for isomorphic pattern building
    static constexpr board_t isomorphic_boards[8] = {
        0x0123456789abcdef,
        0xc840d951ea62fb73,
        0xfedcba9876543210,
        0x37bf26ae159d048c,
        0x32107654ba98fedc,
        0x048c159d26ae37bf,
        0xcdef89ab45670123,
        0xfb73ea62d951c840
    };

public:
    // Length of the pattern
    static constexpr int length = sizeof...(pattern);

private:
    static constexpr int patt[length] = {pattern...};


    // The pattern's isomorphisms
    template <int...Targs>
    struct Isomorphic;
    
    template <int T>
    struct Isomorphic<T> : Isomorphic<T, length> {};

    template <int T, int T1, int...Targs>
    struct Isomorphic<T, T1, Targs...> : Isomorphic<T, T1 - 1, Targs..., Tile(isomorphic_boards[T], patt[length - T1])>{};

    template <int T, int...Targs>
    struct Isomorphic<T, 0, Targs...> {

        static float Estimate(board_t b, float* w) {
            board_t index = 0;
            for (int i : {Targs...}) index = ((index << 4) | Tile(b, i));
            return w[index];
        }

        static float Update(board_t b, float u, float* w, board_t num_weights) {
            float alpha = 1;
            board_t index = 0;
            for (int i : {Targs...}) index = ((index << 4) | Tile(b, i));
#ifdef USE_COHERENCE
            float& error = w[index + num_weights];
            float& abs_error = w[index + 2 * num_weights];
            if (abs_error != 0) alpha = std::abs(error) / abs_error;
#endif
            w[index] += alpha * u;
#ifdef USE_COHERENCE
            error += u;
            abs_error += std::abs(u);
#endif
            return w[index];
        }
    };
public:
     
    static float Estimate(board_t b, float* w) {
        float value = 0;
        value += Isomorphic<0>::Estimate(b, w);
        value += Isomorphic<1>::Estimate(b, w);
        value += Isomorphic<2>::Estimate(b, w);
        value += Isomorphic<3>::Estimate(b, w);
        value += Isomorphic<4>::Estimate(b, w);
        value += Isomorphic<5>::Estimate(b, w);
        value += Isomorphic<6>::Estimate(b, w);
        value += Isomorphic<7>::Estimate(b, w);
        return value;
    }

    static float Update(board_t b, float u, float* w, board_t n) {
        float value = 0;
        value += Isomorphic<0>::Update(b, u, w, n);
        value += Isomorphic<1>::Update(b, u, w, n);
        value += Isomorphic<2>::Update(b, u, w, n);
        value += Isomorphic<3>::Update(b, u, w, n);
        value += Isomorphic<4>::Update(b, u, w, n);
        value += Isomorphic<5>::Update(b, u, w, n);
        value += Isomorphic<6>::Update(b, u, w, n);
        value += Isomorphic<7>::Update(b, u, w, n);
        return value;
    }
};


template <class...Features>
class TupleNetwork {
private:
    
    template <class...Targs> struct Tuples;

    template <class T>
    struct Tuples<T> {
        static constexpr unsigned number_of_weights = (1 << (4 * T::length));

        static float Estimate(board_t b, float* w) { return T::Estimate(b, w); }

        static float Update(board_t b, float u, float* w) { return T::Update(b, u, w, Tuples<Features...>::number_of_weights); }
    };

    template<class T, class...Targs>
    struct Tuples<T, Targs...> {
        static constexpr unsigned number_of_weights = Tuples<T>::number_of_weights + Tuples<Targs...>::number_of_weights;

        static float Estimate(board_t b, float* w) {
            return Tuples<T>::Estimate(b, w) + Tuples<Targs...>::Estimate(b, w + Tuples<T>::number_of_weights);
        }

        static float Update(board_t b, float u, float* w) {
            return Tuples<T>::Update(b, u, w) + Tuples<Targs...>::Update(b, u, w + Tuples<T>::number_of_weights);
        }
    };
public:

    // The weights of the model
#ifndef USE_COHERENCE
    float* weights = new float[Tuples<Features...>::number_of_weights];
#else
    float* weights = new float[Tuples<Features...>::number_of_weights * 3];
#endif

    // Size of the model
    static constexpr unsigned length = sizeof...(Features);

    // Estimate the value of a board
    float Estimate(board_t b) { return Tuples<Features...>::Estimate(b, weights); }

    // Update the value of a board and return the updated value
    float Update(board_t b, float u) {
        u /= 8.0f * sizeof...(Features);
        return Tuples<Features...>::Update(b, u, weights);
    }

    // Save the model to a binary file
    void Save(std::string path) {
        std::ofstream fout(path.c_str(), std::ios::out | std::ios::binary);
        fout.write((char*)weights, sizeof(float) * Tuples<Features...>::number_of_weights);
        fout.close();
#ifdef USE_COHERENCE
        fout.open((path + ".tc").c_str(), std::ios::out | std::ios::binary);
        fout.write((char*)(weights + Tuples<Features...>::number_of_weights), sizeof(float) * Tuples<Features...>::number_of_weights * 2);
        fout.close();
#endif
    }

    // Load the model from a binary file
    void Load(std::string path) {
        std::ifstream fin(path.c_str(), std::ios::out | std::ios::binary);
        if (fin.is_open()) fin.read((char*)weights, sizeof(float) * Tuples<Features...>::number_of_weights);
        fin.close();
#ifdef USE_COHERENCE
        fin.open((path + ".tc").c_str(), std::ios::out | std::ios::binary);
        fin.read((char*)(weights + Tuples<Features...>::number_of_weights), sizeof(float) * Tuples<Features...>::number_of_weights * 2);
        fin.close();
#endif
    }
};

// Predefined structures

typedef TupleNetwork<Pattern<0, 1, 2, 3, 4, 5>, Pattern<4, 5, 6, 7, 8, 9>, Pattern<0, 1, 2, 4, 5, 6>, Pattern<4, 5, 6, 8, 9, 10>> nw4x6;
typedef TupleNetwork<Pattern<0, 1, 2, 3, 4, 5>, Pattern<4, 5, 6, 7, 8, 9>, Pattern<8, 9, 10, 11, 12, 13>, Pattern<0, 1, 2, 4, 5, 6>, Pattern<4, 5, 6, 8, 9, 10>> nw5x6;
typedef TupleNetwork<Pattern<0, 1, 2, 3>, Pattern<4, 5, 6, 7>, Pattern<0, 1, 4, 5>, Pattern<1, 2, 5, 6>, Pattern<5, 6, 9, 10>> nw5x4;
typedef TupleNetwork<Pattern<0, 1, 2, 3, 4>, Pattern<4, 5, 6, 7, 8>, Pattern<8, 9, 10, 11, 12>, Pattern<0, 1, 2, 4, 5>, Pattern<4, 5, 6, 8, 9>, Pattern<8, 9, 10, 12, 13>, Pattern<1, 2, 3, 5, 6>, Pattern<5, 6, 7, 9, 10>, Pattern<9, 10, 11, 13, 14>> nw9x5;

#endif
