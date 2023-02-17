#pragma once

#include <string_view>
#include <functional>

#include <argparse/argparse.hpp>

namespace thyme {

class CLIHandler;

struct HandlerDefinition {
  using SubcommandHandlerFnPtr = auto(thyme::CLIHandler::*)(argparse::ArgumentParser&) const -> void;

  std::string_view subcommand;
  SubcommandHandlerFnPtr handler_fn;
  std::reference_wrapper<argparse::ArgumentParser> parser;
};

struct CLIConfig {
  int argc;
  char** argv;

  std::reference_wrapper<argparse::ArgumentParser> main_parser;

  std::vector<HandlerDefinition> handlers;
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
