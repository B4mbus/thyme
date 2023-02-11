#include <string_view>
#include <chrono>
#include <thread>

#include <process.hpp>

#include "thyme/synchronized_process.hpp"

namespace {

auto create_output_callback(std::string& output_str) {
  return [&output_str](char const* bytes, std::size_t n) {
    output_str.append(std::string_view(bytes, n));
  };
}

} // namespace

namespace thyme {

auto SynchronizedProcess::wait(Millis timeout) -> CommandExecutionResult {
  using Clock = std::chrono::steady_clock;

  auto timed_out = false;
  auto exit_status = -1;
  auto end = Clock::now() + timeout;

  while(not process.try_get_exit_status(exit_status)) {
    if(Clock::now() > end) {
      process.kill();
      timed_out = true;
      break;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  return {
    .stdout = std::move(stdout),
    .stderr = std::move(stderr),
    .timed_out = timed_out,
    .exit_status = exit_status
  };
};

auto SynchronizedProcess::start_process(std::string command) -> TinyProcessLib::Process {
  return TinyProcessLib::Process(
    command,
    "",
    create_output_callback(stdout),
    create_output_callback(stderr)
  );
};

} // namespace thyme
