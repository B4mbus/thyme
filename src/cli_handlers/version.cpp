#include <string_view>
#include <utility>
#include <ranges>
#include <string>
#include <chrono>

#include <fmt/format.h>
#include <fmt/color.h>

#include <argparse/argparse.hpp>

#include <process.hpp>

#include <tl/expected.hpp>

#include "thyme/generated/fennel_scripts.hpp"
#include "thyme/generated/version.hpp"

#include "thyme/formatted_message/messages.hpp"
#include "thyme/synchronized_process.hpp"
#include "thyme/cli_handler.hpp"
#include "thyme/error.hpp"

namespace {

auto split_by_tab(std::string const& tab_delimited_string) {
  auto const tab_index = tab_delimited_string.find('\t');

  auto const first = tab_delimited_string.substr(0, tab_index);
  auto const second = tab_delimited_string.substr(tab_index + 1, tab_delimited_string.size() - tab_index);

  return std::pair(first, second);
}

auto output_from_command(auto cmd, thyme::SynchronizedProcess::Millis timeout) {
  auto proc = thyme::SynchronizedProcess(cmd);
  auto output = proc.wait(timeout);

  std::erase(output.stdout, '\n');

  return output;
}

struct InvocationError {
  std::string stderr;
  int value;

  enum Reason {
    TimedOut,
    NonZeroExitCode,
    EmptyOutput
  } reason;
};

struct VersionInfo {
  VersionInfo(std::string fennel, std::string lua)
    : fennel_version(std::move(fennel)),
      lua_version(std::move(lua)) {}

  std::string fennel_version;
  std::string lua_version;
};

auto get_fennel_and_lua_version_info(thyme::SynchronizedProcess::Millis timeout) -> tl::expected<VersionInfo, InvocationError> {
  auto const version_extraction_script = thyme::fnl::version_extraction();

  auto const version_extraction_cmd = fmt::format(R"(fennel -e "{}")", version_extraction_script);

  auto invocation_result = output_from_command(version_extraction_cmd, timeout);

  if(invocation_result.exit_status != 0) {
    return tl::unexpected(
      InvocationError {
        .stderr = std::move(invocation_result.stderr),
        .value = invocation_result.exit_status,
        .reason = InvocationError::NonZeroExitCode,
      }
    );
  }

  if(invocation_result.timed_out) {
    return tl::unexpected(
      InvocationError {
        .stderr = std::move(invocation_result.stderr),
        .value = static_cast<int>(timeout.count()),
        .reason = InvocationError::TimedOut,
      }
    );
  }

  if(invocation_result.stdout.empty()) {
    return tl::unexpected(
      InvocationError {
        .stderr = std::move(invocation_result.stderr),
        .reason = InvocationError::EmptyOutput,
      }
    );
  }

  auto [fennel_ver, lua_ver] = split_by_tab(invocation_result.stdout);
  return { tl::in_place, fennel_ver, lua_ver };
}

auto print_errors(InvocationError const& error) -> int {
  switch(error.reason) {
    case InvocationError::TimedOut:
      thyme::error("While trying to get fennel and lua version info. Process timed out after {}ms", error.value)
        .write(stderr);
      break;
    case InvocationError::NonZeroExitCode:
      thyme::error("While trying to get fennel and lua version info. Process exited with exit code {}", error.value)
        .hint("Check if the directory where the `fennel` file is located is added to path")
        .hint("Alternatively, configure path to the fennel tool in the buildfile")
        .context("Stderr", "{}", error.stderr)
        .write(stderr);
      break;
    case InvocationError::EmptyOutput:
      thyme::error("While trying to get fennel and lua version info. The output seems to be empty")
        .context("Stderr", "{}", error.stderr)
        .write(stderr);
      break;
  }

  return -1;
}

} // namespace

namespace thyme {

auto CLIHandler::version_handler(argparse::ArgumentParser& parser) const -> int {
  auto constexpr print_version = [](auto name, auto version, auto fg_format) {
    fmt::print(
      "{} version: {}\n",
      fmt::format(fg_format, name),
      fmt::format(fmt::emphasis::bold | fg(fmt::color::light_steel_blue), version)
    );
  };

  print_version("Thyme", thyme::version(), fg(fmt::color::light_sea_green));

  auto const include_fennel = not parser.is_used("--no-fennel");
  auto const include_lua = not parser.is_used("--no-lua");

  if(include_fennel or include_lua) {
    using namespace std::chrono_literals;
    auto const version_info = get_fennel_and_lua_version_info(300ms);

    auto const print_lua_and_fennel_version = [include_fennel, include_lua, print_version](VersionInfo const& version_info) -> int {
      if(include_fennel)
        print_version("Fennel", version_info.fennel_version, fg(fmt::color::light_green));
      if(include_lua)
        print_version("Lua", version_info.lua_version, fg(fmt::color::blue));

      return 0;
    };

    if(version_info)
      return version_info.map(print_lua_and_fennel_version).value();
    else
      return version_info.map_error(print_errors).error();
  }

  return 0;
}

} // namespace thyme
