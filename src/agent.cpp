#include <getopt.h>

#include <algorithm>
#include <cstdlib>
#include <thread>

#include "board.h"
#include "html.h"
#include "search.h"
#include "seed.h"

#ifndef GUI
#define GUI true
#endif

#if GUI
#include "webview/webview.h"
#endif

#ifndef STRUCTURE
#define STRUCTURE nw4x6
#endif

#ifndef FILE_NAME
#define FILE_NAME "weights.bin"
#endif

std::chrono::_V2::high_resolution_clock::time_point start;
bool showboard = false;
int games = 1;
float moves = 0;
int last_done = 0;
bool gui = false;
std::vector<double> scores;
int rate[16] = {0};

Search<STRUCTURE> search;

// Print statistics
void ShowStat(int n) {
    if (!showboard) std::cout << "\033[u";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "progress: " << n << '/' << games;
    std::cout << "\n\033[Kaverage score: " << std::accumulate(scores.begin(), scores.end(), 0.0) / scores.size() << '\n';
    std::cout << "\033[Kmaximum score: " << *std::max_element(scores.begin(), scores.end()) << '\n';
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::cout << "\033[Kspeed: " << moves * 1e9 / (float)duration.count() << " moves per second\n";
    auto accu = 0;
    for (auto i = 15; i > 0; i--) {
        if (!rate[i]) continue;
        accu += rate[i];
        std::cout << "\033[K\t" << (1 << i) << '\t' << accu * 100.f / float(n) << "%\n";
    }
}

void game(int index) {
    auto this_moves = 0;
    board_t board = AddTile(AddTile(0));
    auto score = 0;
    for (;;) {
        if (showboard) {
            std::cout << "\033[u";
            PrintBoard(board);
        }
        auto dir = search(board);
        if (dir < 0) break;
        this_moves++;
        score += move.Score(board, dir);
        board = AddTile(move(board, dir));
    }
    scores.push_back(score);
    rate[MaxRank(board)]++;
    moves += this_moves;
    if (index > last_done) {
        ShowStat(index);
        last_done = index;
    }
}

int main(int argc, char* argv[]) {
    search.Load(FILE_NAME);
    auto c = -1;
    auto thread_count = 1;
    while ((c = getopt(argc, argv, "d:e:tsg")) != -1) switch (c) {
            case 'd': search.min_depth = atoi(optarg); break;
            case 'e': games = atoi(optarg); break;
            case 's': showboard = true; break;
            case 'g': gui = true; break;
            case 't': thread_count = std::thread::hardware_concurrency(); break;
        }
    if (gui) {
#if GUI
        auto fn = [&](std::string s) -> std::string {
            board_t b = 0;
            for (int i = 0; i < 4; i++) b = ((b << 16) | std::stoull(webview::json_parse(s, "", i)));

            return std::to_string(search(b));
        };
        webview::webview w;

        w.bind("AIMove", fn);

        w.set_size(1024, 768, WEBVIEW_HINT_NONE);
        w.set_size(800, 600, WEBVIEW_HINT_MIN);
        w.set_title("2048 Agent");
        w.set_html(html);
        w.run();
#else
        std::cout << "GUI not supported\n";
#endif
        return 0;
    }
    std::cout << "\x1B[2J\x1B[H";
    auto seed = RandomSeed();
    srand(seed);
    std::cout << "seed = " << seed << '\t' << "depth = " << search.min_depth << '\n';
    std::cout << "\033[s";

    // Play N games

    auto n = 1;

    auto pool = std::vector<std::thread>(thread_count);

    auto job = [&]() {
        while (n <= games) {
            game(n++);
        }
    };

    start = std::chrono::high_resolution_clock::now();

    for (auto& thread : pool) thread = std::thread(job);

    for (auto& thread : pool) thread.join();
    std::cout << '\n';
    return 0;
}