#include <getopt.h>

#include <algorithm>
#include <cstdlib>

#include "board.h"
#include "gui.h"
#include "search.h"
#include "seed.h"

#ifndef STRUCTURE
#define STRUCTURE nw4x6
#endif

#ifndef FILE_NAME
#define FILE_NAME "weights.bin"
#endif

bool showboard = false;
int games = 1;
bool gui = false;
std::vector<double> speeds;
std::vector<double> scores;
int rate[16] = {0};

Search<STRUCTURE> search;

// Print statistics
void ShowStat(int n) {
    if (!showboard) std::cout << "\033[u";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "progress: " << n << '/' << games;
    std::cout << "\n\033[Kaverage score: " << std::accumulate(scores.begin(), scores.end(), 0.0) / double(n) << '\n';
    std::cout << "\033[Kmaximum score: " << *std::max_element(scores.begin(), scores.end()) << '\n';
    std::cout << "\033[Kaverage speed: " << std::accumulate(speeds.begin(), speeds.end(), 0.0) / (double)n << " moves per second\n";
    int accu = 0;
    for (int i = 15; i > 0; i--) {
        if (!rate[i]) continue;
        accu += rate[i];
        std::cout << "\033[K\t" << (1 << i) << '\t' << accu * 100.f / float(n) << "%\n";
    }
}

int main(int argc, char* argv[]) {
    search.Load(FILE_NAME);
    int c;
    while ((c = getopt(argc, argv, "d:e:sg")) != -1) switch (c) {
            case 'd': search.min_depth = atoi(optarg); break;
            case 'e': games = atoi(optarg); break;
            case 's': showboard = true; break;
            case 'g': gui = true; break;
        }
    if (gui) {
        auto fn = [&](std::string s) -> std::string {
            board_t b = 0;
            for (int i = 0; i < 4; i++) b = ((b << 16) | std::stoull(webview::json_parse(s, "", i)));

            return std::to_string(search(b));
        };

        start_webview(fn);
        return 0;
    }
    std::cout << "\x1B[2J\x1B[H";
    long long seed = RandomSeed();
    srand(seed);
    std::cout << "seed = " << seed << '\t' << "depth = " << search.min_depth << '\n';
    std::cout << "\033[s";

    // Play N games
    for (int n = 1; n <= games; n++) {
        int moves = 0;
        auto start = std::chrono::high_resolution_clock::now();
        board_t board = AddTile(AddTile(0));
        int score = 0;
        for (;;) {
            if (showboard) {
                std::cout << "\033[u";
                PrintBoard(board);
            }
            int dir = search(board);
            if (dir < 0) break;
            moves++;
            score += move.Score(board, dir);
            board = AddTile(move(board, dir));
        }
        auto end = std::chrono::high_resolution_clock::now();
        speeds.push_back((double)moves * 1e9 / (double)std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
        scores.push_back(score);
        rate[MaxRank(board)]++;
        ShowStat(n);
    }
    std::cout << '\n';
    return 0;
}
