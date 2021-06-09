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
public:

    // Length of the pattern
    static constexpr int length = sizeof...(pattern);

    // The pattern's isomorphisms
    static std::array<std::array<int, length>, 8> isomorphic;

private:

    // Build the pattern's 8 isomorphisms
    static std::array<std::array<int, length>, 8> BuildIsomorphic() {
        board_t isomorphic_boards[8] = {
            0x0123456789abcdef,
            0xc840d951ea62fb73,
            0xfedcba9876543210,
            0x37bf26ae159d048c,
            0x32107654ba98fedc,
            0x048c159d26ae37bf,
            0xcdef89ab45670123,
            0xfb73ea62d951c840
        };
        std::array<std::array<int, length>, 8> isomorphic;
        for (int i = 0; i < 8; i++) {
            board_t idx = isomorphic_boards[i];
            int j = 0;
            for (int x : std::vector<int>{ {pattern...} }) {
                isomorphic[i][j++] = Tile(idx, x);
            }
        }
        return isomorphic;
    }

};

template<int...pattern> std::array<std::array<int, Pattern<pattern...>::length>, 8> Pattern<pattern...>::isomorphic = BuildIsomorphic();


template <class...Features>
class TupleNetwork {
private:
    
    template <class...Targs> struct Tuples;

    template <class T>
    struct Tuples<T> {
        static constexpr unsigned number_of_weights = (1 << (4 * T::length));

        static float Estimate(board_t b, float* w) {
            float value = 0;
            for (int i = 0; i < 8; i++) {
                board_t index = 0;
                for (int j : T::isomorphic[i]) index = ((index << 4) | Tile(b, j));
                value += w[index];
            }
            return value;
        }

        static float Update(board_t b, float u, float* w) {
            float value = 0;
            for (int i = 0; i < 8; i++) {
                board_t index = 0;
                for (int j : T::isomorphic[i]) index = ((index << 4) | Tile(b, j));
                value += w[index];
                w[index] += u;
            }
            return value;
        }

        static std::vector<board_t> GetIndex(board_t b, board_t offset) {
            std::vector<board_t> result;
            for (int i = 0; i < 8; i++) {
                board_t index = 0;
                for (int j : T::isomorphic[i]) index = ((index << 4) | Tile(b, j));
                result.push_back(index + offset);
            }
            return result;
        };
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

        static std::vector<board_t> GetIndex(board_t b, board_t offset) {
            std::vector<board_t> a = Tuples<T>::GetIndex(b, offset);
            std::vector<board_t> ext = Tuples<Targs...>::GetIndex(b, offset + Tuples<T>::number_of_weights);
            a.reserve(ext.size());
            a.insert(a.end(), ext.begin(), ext.end());
            return a;
        }
    };
public:

    // The weights of the model
    float weights[Tuples<Features...>::number_of_weights];

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
        fout.write((char*)((float*)weights), sizeof(float) * Tuples<Features...>::number_of_weights);
        fout.close();
    }

    // Load the model from a binary file
    void Load(std::string path) {
        std::ifstream fin(path.c_str(), std::ios::out | std::ios::binary);
        if (fin.is_open()) fin.read((char*)((float*)weights), sizeof(float) * Tuples<Features...>::number_of_weights);
        fin.close();
    }

    // Get the indexes of all active weights
    std::vector<board_t> GetIndex(board_t b) {
        return Tuples<Features...>::GetIndex(b, 0);
    }
};

typedef TupleNetwork<Pattern<0, 1, 2, 3, 4, 5>, Pattern<4, 5, 6, 7, 8, 9>, Pattern<0, 1, 2, 4, 5, 6>, Pattern<4, 5, 6, 8, 9, 10>> nw4x6tuples;
typedef TupleNetwork<Pattern<0, 1, 2, 3>, Pattern<4, 5, 6, 7>, Pattern<0, 1, 4, 5>, Pattern<1, 2, 5, 6>, Pattern<5, 6, 9, 10>> nw5x4tuples;

#endif //TUPLENET_H
