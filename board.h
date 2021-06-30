#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <iomanip>
#include <chrono>
#include <random>

// Store the board in a 64-bit bitboard
typedef unsigned long long board_t;

// Each row is 16-bit
typedef unsigned short row_t;


// Extract 4-bit tile
#define Tile(b, x) ((b >> ((0xf - x) << 2)) & 0xf)

// Mirror row
#define ReverseRow(row) row_t((row >> 12) | ((row >> 4) & 0x00F0) | ((row << 4) & 0x0F00) | (row << 12))

// Flip board
#define Flip(b) (((b & 0x000000000000ffffULL) << 48) | ((b & 0x00000000ffff0000ULL) << 16) | ((b & 0x0000ffff00000000ULL) >> 16) | ((b & 0xffff000000000000ULL) >> 48))

// Print the board
void PrintBoard(board_t b) {
    std::cout << "+-------+-------+-------+-------+\n";
    for (int i = 0; i < 16; i++) {
        int tile = (1 << Tile(b, i)) & -2;
        if (tile) std::cout << '|' << std::setw(6) << tile << ' ';
        else std::cout << "|       ";
        if (i % 4 == 3) std::cout << "|\n+-------+-------+-------+-------+\n";
    }
}

// All empty positions
board_t EmptyPos(board_t b) {
    b |= (b >> 2) & 0x3333333333333333ULL;
    b |= (b >> 1);
    return ~b & 0x1111111111111111ULL;
}

// Add a random tile
board_t AddTile(board_t b) {
    int num_empty = 0;
    board_t tiles[16] = { 0 };
    board_t mask = EmptyPos(b);
    while (mask) {
        board_t tile = mask & (~mask + 1);
        tiles[num_empty++] = tile;
        mask ^= tile;
    }
    return b | (tiles[rand() % num_empty] << (rand() % 10 == 0));
}

// Highest tile
int MaxRank(board_t b) {
    int max = 0;
    while (b) {
        max = std::max(max, int(b & 0xf));
        b >>= 4;
    }
    return max;
}

// Sum all distinct tiles
board_t Sum(board_t b) {
    unsigned short mask = 0;
    while (b) {
        mask |= (1 << (b & 0xf));
        b >>= 4;
    }
    board_t count = 0;
    while (mask) {
        board_t tile = mask & (~mask + 1);
        count += tile;
        mask ^= tile;
    }
    return count;
}

// Count distinct tiles
int CountDistinct(board_t b) {
    unsigned short mask = 0;
    while (b) {
        mask |= (1 << (b & 0xf));
        b >>= 4;
    }
    int count = 0;
    for (mask >>= 1; mask; mask &= (mask - 1)) count++;
    return count;
}

// Swap row and col
board_t Transpose(board_t x) {
    board_t t;
    t = (x ^ (x >> 12)) & 0x0000f0f00000f0f0ull;
    x ^= t ^ (t << 12);
    t = (x ^ (x >> 24)) & 0x00000000ff00ff00ull;
    x ^= t ^ (t << 24);
    return x;
}

// Movement class
class Move {
public:
    Move() {
        for (unsigned row = 0; row < 65536; ++row) {
            score_table[row] = 0;
            unsigned line[4] = {
                (row >> 0) & 0xf,
                (row >> 4) & 0xf,
                (row >> 8) & 0xf,
                (row >> 12) & 0xf
            };
            int farthest = 3;
            bool merged = false;
            for (int i = 3; i >= 0; --i) {
                if (!line[i]) continue;
                if (!merged && farthest < 3 && line[i] == line[farthest + 1]) {
                    line[farthest + 1] = (line[farthest + 1] + 1) & 0xf;
                    score_table[row] += (1 << line[farthest + 1]);
                    line[i] = 0;
                    merged = true;
                }
                else if (farthest == i) --farthest;
                else {
                    line[farthest--] = line[i];
                    line[i] = 0;
                    merged = false;
                }
            }
            move_table[row] = line[0] | (line[1] << 4) | (line[2] << 8) | (line[3] << 12);
            rev_table[ReverseRow(row)] = ReverseRow(move_table[row]);
        }
    }
    board_t operator()(board_t s, int dir) {
        switch (dir) {
        case 0: return MoveUp(s);
        case 1: return MoveRight(s);
        case 2: return MoveDown(s);
        case 3: return MoveLeft(s);
        default: return s;
        }
    }
    int Score(board_t b, int dir) {
        int score = 0;
        if (!(dir & 1)) b = Transpose(b);
        while (b) {
            score += score_table[b & 0xffff];
            b >>= 16;
        }
        return score;
    }
private:
    row_t move_table[65536];
    row_t rev_table[65536];
    int score_table[65536];
    board_t MoveLeft(board_t s) {
        return (board_t(move_table[s & 0xffff]) |
            (board_t(move_table[(s >> 16) & 0xffff]) << 16) |
            (board_t(move_table[(s >> 32) & 0xffff]) << 32) |
            (board_t(move_table[(s >> 48) & 0xffff]) << 48));
    }

    board_t MoveRight(board_t s) {
        return (board_t(rev_table[s & 0xffff]) |
            (board_t(rev_table[(s >> 16) & 0xffff]) << 16) |
            (board_t(rev_table[(s >> 32) & 0xffff]) << 32) |
            (board_t(rev_table[(s >> 48) & 0xffff]) << 48));
    }

    board_t MoveUp(board_t s) {
        return Transpose(MoveLeft(Transpose(s)));
    }

    board_t MoveDown(board_t s) {
        return Transpose(MoveRight(Transpose(s)));
    }
} move;

#endif
