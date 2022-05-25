#ifndef TRAIN_H
#define TRAIN_H

#include <getopt.h>

#include <future>
#include <iostream>
#include <thread>
#include <tuple>

#include "core/board.h"
#include "core/math.h"
#include "core/movement.h"
#include "core/stat.h"
#include "network/learning.h"
#include "network/tuplenet.h"

#ifndef STRUCTURE
#define STRUCTURE nw4x6
#endif

#ifndef FILE_NAME
#define FILE_NAME "nw4x6/weights.bin"
#endif

namespace train {

inline const auto parse(int argc, char* argv[]) {
  auto c = -1;
  auto games = 1;
  auto alpha = 0.1f;
  auto lambda = 0.5f;

  auto read = false;
  auto write = false;
  auto coherence = false;
  auto restart = false;
  auto thread_count = 1;

  while ((c = getopt(argc, argv, "a:l:e:iocrt:h")) != -1) switch (c) {
      case 'a': alpha = std::atof(optarg); break;
      case 'l': lambda = std::atof(optarg); break;
      case 'e': games = 1000 * std::atoi(optarg); break;
      case 'i': read = true; break;
      case 'o': write = true; break;
      case 'c': coherence = true; break;
      case 'r': restart = true; break;
      case 't':
        thread_count = std::atoi(optarg);
        if (thread_count == 0)
          thread_count = std::thread::hardware_concurrency();
        break;
      case 'h':
      default:
        std::cout
          << "2048 TDL by Zap\n"
             "Released under the MIT License\n\n"
             "Options:\n"
             "  -a <alpha>    -- Set the learning rate\n"
             "                   default: 0.1\n"
             "  -l <lambda>   -- Set the trace decay\n"
             "                   default: 0.5\n"
             "  -e <episodes> -- Set the number of training games * 1000\n"
             "                   default: 1\n"
             "  -t <threads>  -- Set the number of threads\n"
             "                   default: 1 (0 uses all threads)\n"
             "  -i            -- Enable reading from a binary file\n"
             "  -o            -- Enable writing to a binary file\n"
             "  -c            -- Enable temporal coherence learning\n"
             "  -r            -- Enable restart strategy\n"
             "  -h            -- Show this message\n\n"
             "See https://github.com/ziap/2048-tdl for more info\n";
        games = 0;
    }

  return std::tuple{alpha, lambda,    games,   read,
                    write, coherence, restart, thread_count};
}

inline int main(int argc, char* argv[]) {
  const auto
    [alpha, lambda, games, read, write, coherence, restart, thread_count] =
      parse(argc, argv);

  if (games == 0) return 0;

  if (read) {
    if (coherence) STRUCTURE::with_coherence::load(FILE_NAME);
    else
      STRUCTURE::load(FILE_NAME);
  }

  std::cout << "Number of weights: " << STRUCTURE::weight_len << '\n';
  std::cout << "α = " << alpha;
  if (coherence) std::cout << " (TC)\n";
  else
    std::cout << '\n';
  std::cout << "λ = " << lambda << "\n\n";

  const auto learn_task = coherence ? 
    [](int games, float alpha, float lambda, bool restart, int trd_id) {
      learning<STRUCTURE::with_coherence> tdl(alpha, lambda, restart);
      return tdl.learn_episodes(games, trd_id);
    } : [](int games, float alpha, float lambda, bool restart, int trd_id) {
      learning<STRUCTURE> tdl(alpha, lambda, restart);
      return tdl.learn_episodes(games, trd_id);
    };

  const auto games_per_thread = ceil(float(games) / float(thread_count));
  const auto remaining_games = games - games_per_thread * (thread_count - 1);

  std::vector<std::future<stat>> results;

  for (auto i = 1; i < thread_count; i++)
    results.push_back(std::async(
      std::launch::async, learn_task, games_per_thread, alpha, lambda, restart,
      i
    ));

  auto result = learn_task(remaining_games, alpha, lambda, restart, 0);

  for (auto& i : results) result.join(i.get());

  result.print();

  if (coherence) STRUCTURE::with_coherence::save(FILE_NAME);
  else
    STRUCTURE::save(FILE_NAME);
  return 0;
}

}  // namespace train

#endif