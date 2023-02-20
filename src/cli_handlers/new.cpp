#include <fmt/format.h>

#include "thyme/cli_handler.hpp"
#include "thyme/formatted_message/messages.hpp"

namespace thyme {

auto CLIHandler::new_handler(argparse::ArgumentParser& parser) const -> void {
  thyme::error("Unimplemented!").write(stderr);
}

} // namespace thyme
