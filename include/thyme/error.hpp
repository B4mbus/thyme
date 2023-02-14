#pragma once

#include <string>
#include <utility>

#include <fmt/format.h>
#include <fmt/color.h>

namespace thyme {

class Error {
public:
  template<typename... Args>
  Error(fmt::format_string<Args...> desc, Args&&... args)
    : m_desc(fmt::format(desc, std::forward<Args>(args)...)) {}

  template<typename... Args>
  Error(std::string desc_title, fmt::format_string<Args...> desc, Args&&... args)
    : m_desc_title(std::move(desc_title)),
      m_desc(fmt::format(desc, std::forward<Args>(args)...)) {}

private:
  // struct instead of class because apparently it can trigger MSVC linker errors
  friend struct fmt::formatter<thyme::Error>;

  std::string m_desc_title = "Error";
  std::string m_desc;
};

} // namespace thyme

template<>
class fmt::formatter<thyme::Error> {
public:
  auto constexpr parse(format_parse_context& ctx) { return ctx.begin(); }

  template<typename FmtContext>
  auto constexpr format(thyme::Error const& err, FmtContext& ctx) const {
    using namespace fmt;

    auto const format_title = [&ctx](auto const& color, auto const& fmt_string) {
      format_to(ctx.out(), "{}", fmt::format(emphasis::bold | fg(color::light_gray) | bg(color), " {} ", fmt_string));
    };

    auto const format_content = [&ctx](auto const& color, auto const& fmt_string) {
      format_to(ctx.out(), "{}", fmt::format(bg(color), " {} ", fmt_string));
    };

    auto const end = [&ctx] { return format_to(ctx.out(), "\n"); };

    format_title(color::dark_red, err.m_desc_title);
    format_content(color::fire_brick, err.m_desc);
    return end();
  }
};
