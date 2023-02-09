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
    : command(std::move(command)),
      process(start_process()) {}

  auto wait_on_stdout() -> void;
  auto wait_on_stderr() -> void;
  auto wait() -> void;

  std::string stdout;
  std::string stderr;

private:
  auto create_output_callback(auto& latch, auto& output_str);

  auto start_process() -> TinyProcessLib::Process;

  std::string command;

  std::latch stdout_latch = std::latch(1);
  std::latch stderr_latch = std::latch(1);

  TinyProcessLib::Process process;
};

} // namespace thyme