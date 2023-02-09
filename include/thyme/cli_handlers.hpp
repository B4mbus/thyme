#pragma once

namespace argparse {

class ArgumentParser;

} // namespace argparse

namespace thyme::cli_handlers {

auto version(argparse::ArgumentParser const& subcommand) -> void;

} // namespace thyme::cli_handlers
