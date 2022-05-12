#include <getopt.h>

#include <algorithm>
#include <cstdlib>
#include <future>
#include <thread>

#include "board.h"
#include "html.h"
#include "search.h"
#include "seed.h"
#include "stat.h"

#ifndef GUI
#define GUI false
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

Search<STRUCTURE> search;

Stat PlayNGames(unsigned n, unsigned thread) {
    auto interval = n > 100 ? (n / 100) : 1;

    Stat result;

    for (auto i = 1u; i <= n; i++) {
        auto moves = 0.0f;
        auto board = AddTile(AddTile(0));
        auto score = 0.0f;
        for (;;) {
            auto dir = search(board);
            if (dir < 0) break;
            moves++;
            score += move.Score(board, dir);
            board = AddTile(move(board, dir));
        }
        result.Add(board, score, moves);

        if (i % interval == 0) {
            std::cout << "progress: " << i << '/' << n << " \tthread: " << thread << '\n';
            result.Summary();
            std::cout << '\n';
        }
    }

    return result;
}

int main(int argc, char* argv[]) {
    search.Load(FILE_NAME);
    auto c = -1;
    auto games = 1u;
    auto gui = false;
    auto thread_count = 1u;
    while ((c = getopt(argc, argv, "d:e:t:g")) != -1) switch (c) {
            case 'd': search.min_depth = atoi(optarg); break;
            case 'e': games = atoi(optarg); break;
            case 'g': gui = true; break;
            case 't': thread_count = atoi(optarg); break;
        }
    if (gui) {
#if GUI
        webview::webview w;
        std::thread trd;
        
        w.bind("AIMove", [&](std::string s) -> std::string {
            board_t b = 0;
            for (int i = 0; i < 4; i++) b = ((b << 16) | std::stoull(webview::json_parse(s, "", i)));
            if (trd.joinable()) trd.join();
            trd = std::thread([&](board_t x) {
                auto dir = search(x);
                
                w.eval(std::string("move(") + std::to_string(dir) + ")");      
            }, b);
            return "";
        });

        w.set_size(640, 800, WEBVIEW_HINT_FIXED);
        w.set_title("2048 Agent");
        w.set_html(html);
        w.run();
        if (trd.joinable()) trd.join();
#else
        std::cout << "GUI not supported\n";
        return 1;
#endif
        return 0;
    }
    auto seed = RandomSeed();
    srand(seed);
    std::cout << "seed = " << seed << '\t' << "depth = " << search.min_depth << '\n';

    std::vector<std::future<Stat>> futures;

    for (auto i = 0u; i < thread_count - 1; i++) futures.push_back(std::async(std::launch::async, PlayNGames, games / thread_count, i));

    auto result = PlayNGames(games - games / thread_count * (thread_count - 1), thread_count - 1);

    for (auto& f : futures) { result = result.Join(f.get()); }

    result.Print();
}