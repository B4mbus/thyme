#pragma once

namespace argparse {

class ArgumentParser;

} // namespace argparse

namespace thyme {

class CLIHandler {
public:
  auto default_handler(argparse::ArgumentParser& parser) const -> int;

  auto version_handler(argparse::ArgumentParser& parser) const -> int;
  auto new_handler(argparse::ArgumentParser& parser) const -> int;
};

} // namespace thyme
