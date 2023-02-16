#include <fmt/format.h>

#include <argparse/argparse.hpp>

#include "thyme/cli_handlers.hpp"
#include "thyme/generated/version.hpp"
#include "thyme/thyme.hpp"

namespace thyme {

auto Thyme::run(int argc, char** argv) -> int {
  namespace ap = argparse;

  auto program = ap::ArgumentParser("thyme", thyme::gen::version(), ap::default_arguments::help);

  auto version_subcommand = ap::ArgumentParser("version", "", ap::default_arguments::help);
  version_subcommand.add_description("Displays thyme's, fennel's and lua's version if it can find one.");

  version_subcommand
    .add_argument("-f", "--no-fennel")
    .help("Don't display fennel's version")
    .default_value(false)
    .implicit_value(true);

  version_subcommand
    .add_argument("-l", "--no-lua")
    .help("Don't display lua's version")
    .default_value(false)
    .implicit_value(true);

  program.add_subparser(version_subcommand);

  try {
    program.parse_args(argc, argv);

    if(program.is_subcommand_used("version")) {
      thyme::cli_handlers::version(version_subcommand);
    }
  } catch(std::runtime_error const& err) {
    fmt::print("{}\n{}", err.what(), program.help().str());
    return -1;
  }

  return 0;
}

} // namespace thyme
