#include <string_view>
#include <ranges>
#include <utility>
#include <string>
#include <latch>

#include <fmt/format.h>
#include <fmt/color.h>

#include <argparse/argparse.hpp>

#include <process.hpp>

#include "thyme/synchronized_process.hpp"
#include "thyme/cli_handlers.hpp"
#include "thyme/version.hpp"

namespace {

auto versions_from_str(std::string_view tab_delimited_string) {
  auto const tab_index = tab_delimited_string.find('\t');

  auto const first = tab_delimited_string.substr(0, tab_index);
  auto const second = tab_delimited_string.substr(tab_index + 1, tab_delimited_string.size() - tab_index);

  return std::pair(first, second);
}

auto output_from_command(auto cmd) {
  auto proc = thyme::SynchronizedProcess(cmd);
  auto output = proc.wait();
  std::erase(output.stdout, '\n');

  return output.stdout;
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
    auto const version_extraction_script = R"fennel(
      (fn fst [elems] (. elems 1))
      (fn snd [elems] (. elems 2))

      (fn split-by-space [string]
        (icollect [word (string.gmatch string :%S+)] word))

      (fn luajit-version []
        (-?>
          _G.jit
          (. :version)
          (split-by-space)
          (fst)))

      (fn lua-version []
        (->
          _VERSION
          (split-by-space)
          (snd)))

      (print
        (. (require :fennel) :version)
        (or (luajit-version) (lua-version)))
    )fennel";

    auto const version_extraction_cmd = fmt::format(R"(fennel -e "{}")", version_extraction_script);

    auto const cmd_output = output_from_command(version_extraction_cmd);
    auto const [fennel_version, lua_version] = versions_from_str(cmd_output);

    if(include_fennel) {
      print_version("Fennel", fennel_version, fg(fmt::color::light_green));
    }
    if(include_lua) {
      print_version("Lua", lua_version, fg(fmt::color::blue));
    }
  }
}

} // namespace thyme::cli_handlers
