#pragma once

namespace argparse {

class ArgumentParser;

} // namespace argparse

namespace thyme {

class CLIHandler {
public:
  auto default_handler(argparse::ArgumentParser& parser) const -> void;

  auto version_handler(argparse::ArgumentParser& parser) const -> void;
  auto new_handler(argparse::ArgumentParser& parser) const -> void;
};

} // namespace thyme
