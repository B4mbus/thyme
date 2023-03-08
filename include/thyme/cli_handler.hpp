#pragma once

#include <string_view>
#include <functional>

namespace argparse {

class ArgumentParser;

} // namespace argparse

namespace thyme {

class CLIHandler {
  using HandlerFnPtr = auto(thyme::CLIHandler::*)(argparse::ArgumentParser&) const -> int;

  struct HandlerDefinition {
    HandlerFnPtr handler_fn;
    std::reference_wrapper<argparse::ArgumentParser> parser;
  };

  struct NamedHandlerDefinition {
    std::string_view subcommand;
    HandlerDefinition definition;
  };

  struct CLIConfig {
    int argc;
    char** argv;

    std::reference_wrapper<argparse::ArgumentParser> main_parser;

    HandlerDefinition default_handler;
    std::vector<NamedHandlerDefinition> subcommands_handlers;
  };

public:
  static auto handle_cli(int argc, char** argv) -> int;

  auto default_handler(argparse::ArgumentParser& parser) const -> int;

  auto version_handler(argparse::ArgumentParser& parser) const -> int;
  auto new_handler(argparse::ArgumentParser& parser) const -> int;

private:
  static auto dispatch_handlers(CLIConfig& config) -> int;
};

} // namespace thyme
