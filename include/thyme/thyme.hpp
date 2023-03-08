#pragma once

#include <string_view>
#include <functional>

#include <argparse/argparse.hpp>

#include "thyme/cli_handler.hpp"

namespace thyme {

class Thyme {
public:
  Thyme() = default;

  Thyme(Thyme const&) = delete;
  Thyme(Thyme&&) = delete;

  auto operator=(Thyme const&) = delete;
  auto operator=(Thyme&&) = delete;

  ~Thyme() = default;

  auto run(int argc, char** argv) -> int;
};

} // namespace thyme
