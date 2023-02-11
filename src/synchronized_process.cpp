#include <string_view>

#include <process.hpp>

#include "thyme/synchronized_process.hpp"

namespace thyme {

auto SynchronizedProcess::create_output_callback(std::string& output_str) {
  return [this, &output_str](char const* bytes, std::size_t n) {
    output_str.append(std::string_view(bytes, n));
    process_latch.count_down();
  };
}

auto SynchronizedProcess::wait() -> CommandOutput {
  process_latch.wait();

  return {
    .stdout = std::move(stdout),
    .stderr = std::move(stderr),
    .exit_status = process.get_exit_status()
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
