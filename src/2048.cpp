#include <string>

#include "agent.h"
#include "train.h"

int main(int argc, char *argv[]) {
  const auto help = R"(2048 TDL by Zap
Released under the MIT License

Options:

  train -- Train the model with TDL(λ)
  agent -- Test the model with Expectimax Search

See https://github.com/ziap/2048-tdl for more info
)";

  if (argc > 1) {
    const auto arg = std::string{argv[1]};

    if (arg == "train") return train::main(argc - 1, argv + 1);
    if (arg == "agent") return agent::main(argc - 1, argv + 1);
    std::cout << help;
  } else
    std::cout << help;
}
