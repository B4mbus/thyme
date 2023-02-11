#pragma once

#include <string_view>
#include <utility>
#include <string>
#include <latch>

#include <process.hpp>

namespace thyme {

// TODO: implement timeout
class SynchronizedProcess {
public:
  SynchronizedProcess(std::string command)
    : process(start_process(std::move(command))) {}

  struct CommandOutput {
    std::string stdout;
    std::string stderr;
    int exit_status;
  };

  auto wait() -> CommandOutput;

private:
  auto start_process(std::string command) -> TinyProcessLib::Process;
  auto create_output_callback(std::string& output_str);

  std::string stdout;
  std::string stderr;

  std::latch process_latch = std::latch(1);

  TinyProcessLib::Process process;
};

} // namespace thyme
