#include <string_view>
#include <ranges>
#include <utility>
#include <string>
#include <latch>

#include <fmt/format.h>
#include <fmt/color.h>

#include <argparse/argparse.hpp>

#include <process.hpp>

#include "thyme/cli_handlers.hpp"
#include "thyme/version.hpp"

namespace {

auto extract_versions_from_str(std::string_view tab_delimited_string) {
  auto const tab_index = tab_delimited_string.find('\t');

  auto const first = tab_delimited_string.substr(0, tab_index);
  auto const second = tab_delimited_string.substr(tab_index + 1, tab_delimited_string.size() - tab_index);

  return std::pair(first, second);
}

auto extract_output_from_command(auto cmd) {
  // TODO: implement timeout and extract to a class handling both stdout and stderr
  auto proc_output = std::string();
  auto proc_latch = std::latch(1);

  auto proc = TinyProcessLib::Process(
    cmd,
    "",
    [&proc_output, &proc_latch](char const* bytes, std::size_t n) {
      proc_output.append(std::string_view(bytes, n));
      std::erase(proc_output, '\n');

      proc_latch.count_down();
    }
  );

  proc_latch.wait();
  return proc_output;
}

} // namespace

namespace thyme::cli_handlers {

auto handle_version(argparse::ArgumentParser const& subcommand) -> void {
  auto constexpr print_version = [](auto name, auto version, auto fg_format) {
    fmt::print(
      "{} version: {}\n",
      fmt::format(fg_format, name),
      fmt::format(fmt::emphasis::bold | fg(fmt::color::light_steel_blue), version)
    );
  };

  print_version("Thyme", thyme::meta::version(), fg(fmt::color::light_sea_green));

  auto const include_fennel = not subcommand.is_used("--no-fennel");
  auto const include_lua = not subcommand.is_used("--no-lua");

  if(include_fennel or include_lua) {
    auto const version_extraction_cmd = R"VERSION(
      fennel -e "(print (. (require :fennel) :version) (. (icollect [string (string.gmatch _VERSION :%S+)] (if (~= string :Lua) string)) 1))"
    )VERSION";

    auto const cmd_output = extract_output_from_command(version_extraction_cmd);
    auto const [fennel_version, lua_version] = extract_versions_from_str(cmd_output);

    if(include_fennel) {
      print_version("Fennel", fennel_version, fg(fmt::color::light_green));
    }
    if(include_lua) {
      print_version("Lua", lua_version, fg(fmt::color::blue));
    }
  }
}

} // namespace thyme::cli_handlers
