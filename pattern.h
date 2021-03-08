#ifndef PATTERN_H
#define PATTERN_H
#include <vector>
#include <array>

template<int...pattern>
class Feature {
private:

    // Get the name of the pattern
    static const std::string GetName() {
        std::string name = "";
        std::string hex = "0123456789ABCDEF";
        for (int i : std::vector<int>{ {pattern...} }) name.push_back(hex[i]);
        return name;
    };

    // Build the pattern's 8 isomorphisms
    static std::array<std::vector<int>, 8> BuildIsomorphic() {
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
        std::array<std::vector<int>, 8> isomorphic;
        for (int i = 0; i < 8; i++) {
            board_t idx = isomorphic_boards[i];
            for (int x : std::vector<int>{ {pattern...} }) {
                isomorphic[i].push_back(Tile(idx, x));
            }
        }
        return isomorphic;
    }
public:
    static constexpr int len = sizeof...(pattern);
    static float weights[1 << (4 * len)];
    static const std::string name;
    static std::array<std::vector<int>, 8> isomorphic;

    // Estimate the value of a board
    static float Estimate(board_t b) {
        float value = 0;
        for (int i = 0; i < 8; i++) {
            board_t index = 0;
            for (int j : isomorphic[i]) index = ((index << 4) | Tile(b, j));
            value += weights[index];
        }
        return value;
    }

    // Update a given board and return it's updated value
    static float Update(board_t b, float u) {
        u /= 8;
        float value = 0;
        for (int i = 0; i < 8; i++) {
            board_t index = 0;
            for (int j : isomorphic[i]) index = ((index << 4) | Tile(b, j));
            weights[index] += u;
            value += weights[index];
        }
        return value;
    }
};

template<int...pattern> float Feature<pattern...>::weights[1 << (4 * len)] = {0};
template<int...pattern> const std::string Feature<pattern...>::name = GetName();
template<int...pattern> std::array<std::vector<int>, 8> Feature<pattern...>::isomorphic = BuildIsomorphic();

#endif // PATTERN_H
