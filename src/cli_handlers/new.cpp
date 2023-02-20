#include <fmt/format.h>

#include "thyme/cli_handler.hpp"
#include "thyme/formatted_message/messages.hpp"

namespace thyme {

auto CLIHandler::new_handler(argparse::ArgumentParser& parser) const -> int {
  thyme::error("Unimplemented!").write(stderr);
  return 0;
}

} // namespace thyme
