#include <filesystem>
#include <functional>
#include <vector>

#include <fmt/format.h>

#include <argparse/argparse.hpp>

#include "thyme/generated/version.hpp"

#include "thyme/formatted_message/messages.hpp"
#include "thyme/cli_handler.hpp"
#include "thyme/thyme.hpp"

namespace thyme {

auto Thyme::run(int argc, char** argv) -> int {
  return handle_cli(argc, argv);
}

auto Thyme::handle_cli(int argc, char** argv) -> int {
  namespace ap = argparse;
  namespace fs = std::filesystem;

  auto program = ap::ArgumentParser("thyme", thyme::version(), ap::default_arguments::help);

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

  auto new_subcommand = ap::ArgumentParser("new", "", ap::default_arguments::help);
  new_subcommand.add_description("Initializes a new fennel project.");

  new_subcommand
    .add_argument("project-dir")
    .help("Path to the project directory")
    .default_value(fs::path("."))
    .required();

  new_subcommand
    .add_argument("src")
    .help("The name of the source directory ")
    .default_value(fs::path("fnl"))
    .required();

  new_subcommand
    .add_argument("out")
    .help("The name of the source directory ")
    .default_value(fs::path("lua"))
    .required();

  new_subcommand
    .add_argument("-n", "--no-git")
    .help("Don't do `git init` in the project dir.")
    .default_value(false)
    .implicit_value(true);

  program.add_subparser(new_subcommand);
  program.add_subparser(version_subcommand);

  auto cli_config = CLIConfig {
    .argc = argc,
    .argv = argv,
    .main_parser = std::ref(program),
    .default_handler = { &CLIHandler::default_handler, std::ref(program) },
    .subcommands_handlers = {
      { "version", { &CLIHandler::version_handler, std::ref(version_subcommand) } },
      { "new", { &CLIHandler::new_handler, std::ref(new_subcommand) } },
    }
  };

  return dispatch_handlers(cli_config);
}

auto Thyme::dispatch_handlers(CLIConfig& config) -> int {
  auto& main_parser = config.main_parser.get();
  auto cli_handler = CLIHandler();

  if(config.argc == 1) {
    auto const& [handler_fn, parser] = config.default_handler;
    std::invoke(handler_fn, cli_handler, parser);
  }

  try {
    main_parser.parse_args(config.argc, config.argv);

  } catch(std::runtime_error const& err) {
    thyme::error("{}", err.what())
      .write(stderr);

    fmt::print("{}", main_parser.help().str());
    return -1;
  }

  for(auto&& [subcommand, handler_def] : config.subcommands_handlers) {
    if(main_parser.is_subcommand_used(subcommand))
      std::invoke(handler_def.handler_fn, cli_handler, handler_def.parser);
  }

  return 0;
}

} // namespace thyme
