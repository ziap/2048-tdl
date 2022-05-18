#ifndef AGENT_H
#define AGENT_H

#include <getopt.h>

#include <cstdlib>
#include <future>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <tuple>

#include "core/board.h"
#include "core/math.h"
#include "core/movement.h"
#include "core/stat.h"
#include "network/tuplenet.h"
#include "search/expectimax.h"

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
#define FILE_NAME "nw4x6/weights.bin"
#endif

extern "C" char _binary_src_gui_html_start;
extern "C" char _binary_src_gui_html_end;

namespace agent {

inline const auto parse(int argc, char *argv[]) {
  auto c = -1;
  auto games = 1;
  auto depth = 0;
  auto gui = false;
  auto thread_count = 1;

  while ((c = getopt(argc, argv, "d:e:t:gh")) != -1) switch (c) {
      case 'd': depth = std::atoi(optarg); break;
      case 'e': games = std::atoi(optarg); break;
      case 't':
        thread_count = std::atoi(optarg);
        if (thread_count == 0)
          thread_count = std::thread::hardware_concurrency();
        break;
      case 'g': gui = true; break;
      case 'h':
      default:
        std::cout << "2048 TDL by Zap\n"
                     "Released under the MIT License\n\n"
                     "Options:\n"
                     "  -d <depth>    -- Set the search depth\n"
                     "                   default: 0\n"
                     "  -e <episodes> -- Set the number of games to play\n"
                     "                   default: 1\n"
                     "  -t <threads>  -- Set the number of threads\n"
                     "                   default: 1 (0 uses all threads)\n"
                     "  -g            -- Enable the GUI\n"
                     "  -h            -- Show this message\n\n"
                     "See https://github.com/ziap/2048-tdl for more info\n";
        games = 0;
    }

  return std::tuple{depth, games, gui, thread_count};
}

inline int main(int argc, char *argv[]) {
  const auto [depth, games, gui, thread_count] = parse(argc, argv);

  if (games == 0) return 0;

  STRUCTURE::load(FILE_NAME);

  if (gui) {
#if GUI
    search<STRUCTURE> model(depth);
    webview::webview w;
    math::random rng;
    std::thread trd;
    std::string html = &_binary_src_gui_html_start;

    w.bind("AIMove", [&trd, &model, &w](std::string s) -> std::string {
      board::t b = 0;
      for (int i = 0; i < 4; i++)
        b = ((b << 16) | std::stoull(webview::json_parse(s, "", i)));
      if (trd.joinable()) trd.join();

      trd = std::thread(
        [&model, &w](board::t x) {
          const auto dir = model.suggest_dir(x);

          if (dir != -1)
            w.eval(std::string("move(") + std::to_string(dir) + ")");
        },
        b);

      return "";
    });

    w.set_size(640, 800, WEBVIEW_HINT_FIXED);
    w.set_title("2048");
    w.set_html(html);
    w.run();

    if (trd.joinable()) trd.join();
    return 0;
#else
    std::cout << "The program isn't compiled with a webview GUI\n";
    return 1;
#endif
  }

  const auto play_game = [](
                           math::u32 games, math::u32 depth, math::u32 trd_id) {
    const auto interval = games > 100 ? games / 100 : 1;
    const auto digits = std::to_string(games).length();

    math::random rng;
    stat result;

    search<STRUCTURE> model(depth);

    for (auto i = 1u; i <= games; i++) {
      auto move_cnt = 0;
      auto score = 0.0f;
      auto b = board::add_tile(board::add_tile(0, rng), rng);

      for (;;) {
        auto next = model.suggest_move(b);
        if (next.first != b) {
          b = board::add_tile(next.first, rng);
          score += next.second;
          move_cnt++;
        } else
          break;
      }

      result.add(b, score, move_cnt);

      if (i % interval == 0) {
        std::cout << "progress: " << std::setw(digits) << i << '/' << games
                  << "    thread: #" << trd_id << '\n';
        result.summary();
      }
    }

    return result;
  };

  const auto games_per_thread = ceil(float(games) / float(thread_count));
  const auto remaining_games = games - games_per_thread * (thread_count - 1);

  std::vector<std::future<stat>> results;

  for (auto i = 1; i < thread_count; i++)
    results.push_back(
      std::async(std::launch::async, play_game, games_per_thread, depth, i));

  auto result = play_game(remaining_games, depth, 0);

  for (auto &i : results) result.join(i.get());

  result.print();
  return 0;
}

}  // namespace agent

#endif