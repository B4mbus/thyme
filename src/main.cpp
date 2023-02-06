#include <fmt/format.h>

#include "thyme/version.hpp"

auto main() -> int {
  fmt::print("Version: {}\n", thyme::meta::version());
}
