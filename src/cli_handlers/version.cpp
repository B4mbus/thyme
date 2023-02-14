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
#include "thyme/cli_handlers.hpp"
#include "thyme/version.hpp"

namespace {

auto split_by_tab(std::string_view tab_delimited_string) {
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

  enum {
    TimedOut,
    NonZeroExitCode
  } type;
};

struct VersionInfo {
  VersionInfo(std::string fennel, std::string lua)
    : fennel_version(std::move(fennel)),
      lua_version(std::move(lua)) {}

  std::string fennel_version;
  std::string lua_version;
};

auto get_fennel_and_lua_version_info(thyme::SynchronizedProcess::Millis timeout) -> tl::expected<VersionInfo, InvocationError> {
  auto const version_extraction_script = R"fennel(
    (fn first [elems] (. elems 1))
    (fn second [elems] (. elems 2))

    (fn split-by-space [string]
      (icollect [word (string.gmatch string :%S+)] word))

    (fn luajit-version []
      (-?>
        _G.jit
        (. :version)
        (split-by-space)
        (first)))

    (fn lua-version []
      (->
        _VERSION
        (split-by-space)
        (second)))

    (print
      (. (require :fennel) :version)
      (or (luajit-version) (lua-version)))
    )fennel";

  auto const version_extraction_cmd = fmt::format(R"(fennel -e "{}")", version_extraction_script);

  auto invocation_result = output_from_command(version_extraction_cmd, timeout);

  if(invocation_result.exit_status != 0)
    return tl::unexpected(InvocationError { std::move(invocation_result.stderr), invocation_result.exit_status, InvocationError::NonZeroExitCode });

  if(invocation_result.timed_out)
    return tl::unexpected(InvocationError { std::move(invocation_result.stderr), 300, InvocationError::TimedOut });

  auto const [fennel_ver, lua_ver] = split_by_tab(invocation_result.stdout);
  return { tl::in_place, std::string(fennel_ver), std::string(lua_ver) };
}

} // namespace

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

    if(include_fennel) {
      if(version_info) {
        print_version("Fennel", version_info.value().fennel_version, fg(fmt::color::light_green));
      } else {
        auto const& vers = version_info.error();
        fmt::print("Error: {} Value: {} Stderr: \"{}\"", vers.type == InvocationError::NonZeroExitCode ? "NonZeroExitCode" : "TimedOut", vers.value, vers.stderr);
      }
    }
    if(include_lua) {
      if(version_info) {
        print_version("Lua", version_info.value().lua_version, fg(fmt::color::blue));
      } else {
        auto const& vers = version_info.error();
        fmt::print("Error: {} Value: {} Stderr: \"{}\"", vers.type == InvocationError::NonZeroExitCode ? "NonZeroExitCode" : "TimedOut", vers.value, vers.stderr);
      }
    }
  }
}

} // namespace thyme::cli_handlers
