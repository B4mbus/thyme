#include <filesystem>
#include <functional>
#include <vector>

#include <argparse/argparse.hpp>

#include "thyme/cli_handler.hpp"
#include "thyme/thyme.hpp"

namespace thyme {

auto Thyme::run(int argc, char** argv) -> int {
  return CLIHandler::handle_cli(argc, argv);
}

} // namespace thyme
