#include <fmt/format.h>

#include <argparse/argparse.hpp>

#include "thyme/cli_handler.hpp"

namespace thyme {

auto CLIHandler::default_handler(argparse::ArgumentParser& parser) const -> void {
  fmt::print("{}", parser.help().str());
}

} // namespace thyme
