#include <string_view>
#include <ranges>
#include <utility>
#include <string>
#include <chrono>

#include <fmt/format.h>
#include <fmt/color.h>

#include <argparse/argparse.hpp>

#include <process.hpp>

#include <tl/expected.hpp>

#include "thyme/synchronized_process.hpp"
#include "thyme/fennel_scripts.hpp"
#include "thyme/cli_handlers.hpp"
#include "thyme/version.hpp"
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
    NonZeroExitCode
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

  auto [fennel_ver, lua_ver] = split_by_tab(invocation_result.stdout);
  return { tl::in_place, fennel_ver, lua_ver };
}

} // namespace

template<>
struct fmt::formatter<InvocationError::Reason> : formatter<std::string_view> {
  template<typename FormatContext>
  auto format(InvocationError::Reason reason, FormatContext& ctx) {
    using enum InvocationError::Reason;

    switch(reason) {
      case TimedOut: return fmt::format_to(ctx.out(), "TimedOut");
      case NonZeroExitCode: return fmt::format_to(ctx.out(), "NonZeroExitCode");
    }
  }
};

namespace thyme::cli_handlers {

auto version(argparse::ArgumentParser const& subcommand) -> void {
  auto constexpr print_version = [](auto name, auto version, auto fg_format) {
    fmt::print(
      "{} version: {}\n",
      fmt::format(fg_format, name),
      fmt::format(fmt::emphasis::bold | fg(fmt::color::light_steel_blue), version)
    );
  };

  print_version("Thyme", thyme::gen::version(), fg(fmt::color::light_sea_green));

  auto const include_fennel = not subcommand.is_used("--no-fennel");
  auto const include_lua = not subcommand.is_used("--no-lua");

  if(include_fennel or include_lua) {
    using namespace std::chrono_literals;
    auto const version_info = get_fennel_and_lua_version_info(300ms);

    auto const print_versions = [include_fennel, include_lua, print_version](VersionInfo const& version_info) {
      if(include_fennel)
        print_version("Fennel", version_info.fennel_version, fg(fmt::color::light_green));
      if(include_lua)
        print_version("Lua", version_info.lua_version, fg(fmt::color::blue));
    };

    auto const print_errors = [](InvocationError const& error) {
      switch(error.reason) {
        case InvocationError::TimedOut:
          fmt::print("{}", thyme::Error("While trying to get version info. Process timed out after {}ms.", error.value));
          break;
        case InvocationError::NonZeroExitCode:
          fmt::print("{}", thyme::Error("While trying to get version info. Process exited with exit code {}.", error.value));
          break;
      }
    };

    version_info
      .map(print_versions)
      .map_error(print_errors);
  }
}

} // namespace thyme::cli_handlers
