#pragma once

#include <string_view>
#include <utility>
#include <string>

#include <process.hpp>

namespace thyme {

// TODO: implement timeout
class SynchronizedProcess {
public:
  using Millis = std::chrono::milliseconds;

  SynchronizedProcess(std::string command)
    : process(start_process(std::move(command))) {}

  struct CommandExecutionResult {
    std::string stdout;
    std::string stderr;
    bool timed_out = false;
    int exit_status = -1;
  };

  auto wait(Millis timeout = Millis::max()) -> CommandExecutionResult;

private:
  auto start_process(std::string command) -> TinyProcessLib::Process;

  std::string stdout;
  std::string stderr;

  TinyProcessLib::Process process;
};

} // namespace thyme
