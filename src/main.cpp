#include "thyme/thyme.hpp"

auto main(int argc, char** argv) -> int {
  auto thyme = thyme::Thyme();
  return thyme.run(argc, argv);
}
