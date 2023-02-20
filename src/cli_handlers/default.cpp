#include <fmt/format.h>

#include <argparse/argparse.hpp>

#include "thyme/cli_handler.hpp"

namespace thyme {

auto CLIHandler::default_handler(argparse::ArgumentParser& parser) const -> int {
  fmt::print("{}", parser.help().str());
  return 0;
}

} // namespace thyme
