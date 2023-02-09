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

  auto wait_on_stdout() -> void {
    stdout_latch.wait();
  };

  auto wait_on_stderr() -> void {
    stderr_latch.wait();
  };

  auto wait() -> void {
    wait_on_stdout();
    wait_on_stderr();
  };

  std::string stdout;
  std::string stderr;

private:
  auto create_output_callback(auto& latch, auto& output_str) {
    return [&latch, &output_str](char const* bytes, std::size_t n) {
      output_str.append(std::string_view(bytes, n));
      latch.count_down();
    };
  }

  auto start_process() -> TinyProcessLib::Process {
    return TinyProcessLib::Process(
      command,
      "",
      create_output_callback(stdout_latch, stdout),
      create_output_callback(stderr_latch, stderr)
    );
  };

  std::string command;

  std::latch stdout_latch = std::latch(1);
  std::latch stderr_latch = std::latch(1);

  TinyProcessLib::Process process;
};

} // namespace thyme
