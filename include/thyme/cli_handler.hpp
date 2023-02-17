#pragma once

namespace argparse {

class ArgumentParser;

} // namespace argparse

namespace thyme {

class CLIHandler {
public:
  auto version(argparse::ArgumentParser& parser) const -> void;
};

} // namespace thyme
