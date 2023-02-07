#include <fmt/format.h>
#include <fmt/color.h>

#include <argparse/argparse.hpp>

#include "thyme/version.hpp"

auto main(int argc, char** argv) -> int {
  namespace ap = argparse;

  auto program = ap::ArgumentParser("thyme", thyme::meta::version(), ap::default_arguments::help);

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
      auto constexpr print_version = [](auto name, auto version, auto fg_format) {
        fmt::print(
          "{} version: {}\n",
          fmt::format(fg_format, name),
          fmt::format(fmt::emphasis::bold | fg(fmt::color::light_steel_blue), version)
        );
      };

      print_version("Thyme", thyme::meta::version(), fg(fmt::color::light_sea_green));
      if(not version_subcommand.is_used("--no-fennel")) {
        print_version("Fennel", "?", fg(fmt::color::light_green));
      }
      if(not version_subcommand.is_used("--no-lua")) {
        print_version("Lua", "?", fg(fmt::color::blue));
      }
    }
  } catch(std::runtime_error const& err) {
    fmt::print("{}\n{}", err.what(), program.help().str());
    return -1;
  }
}
