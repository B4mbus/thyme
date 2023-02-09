#include <string_view>

#include <process.hpp>

#include "thyme/synchronized_process.hpp"

namespace {

auto create_output_callback(auto& latch, auto& output_str) {
  return [&latch, &output_str](char const* bytes, std::size_t n) {
    output_str.append(std::string_view(bytes, n));
    latch.count_down();
  };
}

} // namespace

namespace thyme {

auto SynchronizedProcess::wait() -> void {
  wait_on_stdout();
  wait_on_stderr();
};

auto SynchronizedProcess::wait_on_stdout() -> void {
  stdout_latch.wait();
};

auto SynchronizedProcess::wait_on_stderr() -> void {
  stderr_latch.wait();
};

auto SynchronizedProcess::start_process() -> TinyProcessLib::Process {
  return TinyProcessLib::Process(
    command,
    "",
    create_output_callback(stdout_latch, stdout),
    create_output_callback(stderr_latch, stderr)
  );
};

} // namespace thyme
