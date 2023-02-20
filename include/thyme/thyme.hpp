#pragma once

#include <string_view>
#include <functional>

#include <argparse/argparse.hpp>

#include "thyme/cli_handler.hpp"

namespace thyme {

struct HandlerDefinition {
  CLIHandler::HandlerFnPtr handler_fn;
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

class Thyme {
public:
  Thyme() = default;

  Thyme(Thyme const&) = delete;
  Thyme(Thyme&&) = delete;

  auto operator=(Thyme const&) = delete;
  auto operator=(Thyme&&) = delete;

  ~Thyme() = default;

  auto run(int argc, char** argv) -> int;

private:
  auto handle_cli(int argc, char** argv) -> int;

  auto dispatch_handlers(CLIConfig& config) -> int;
};

} // namespace thyme
