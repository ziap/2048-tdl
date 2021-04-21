#ifndef TUPLENET_H
#define TUPLENET_H

#include "pattern.h"

template<class...Features>
class Tuples {
private:
    template<class...Targs> struct TupleNetwork;
    template<class T>
    struct TupleNetwork<T> {

        // Estimate the value of a board
        static float Estimate(board_t b) {
            return T::Estimate(b);
        }

        // Load the model from a binary file
        static void Load(std::istream& in, std::string path) {
            T::Load(in, path);
        }
    };
    template<class T, class...Targs>
    struct TupleNetwork<T, Targs...> {
        static float Estimate(board_t b) {
            return TupleNetwork<T>::Estimate(b) + TupleNetwork<Targs...>::Estimate(b);
        }
        static void Load(std::istream& in, std::string path) {
            TupleNetwork<T>::Load(in, path);
            TupleNetwork<Targs...>::Load(in, path);
        }
    };
public:
    static float Estimate(board_t b) {
        return TupleNetwork<Features...>::Estimate(b);
    }

    static void Load(std::string file_name) {
        std::ifstream fin(file_name.c_str(), std::ios::out | std::ios::binary);
        if (fin.is_open()) TupleNetwork<Features...>::Load(fin, file_name);
    }
};

#endif // TUPLENET_H