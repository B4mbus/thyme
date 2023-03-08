#include <cassert>

#include <fmt/format.h>
#include <fmt/color.h>

#include "thyme/formatted_message/formatted_message.hpp"

namespace thyme {

template<fmt::color TitleStyle, fmt::color BodyStyle>
struct StylePair {
  fmt::color title = TitleStyle;
  fmt::color body = BodyStyle;
};

template<fmt::color T, fmt::color U>
using HintStyle = StylePair<T, U>;

template<fmt::color T, fmt::color U>
using WarningStyle = StylePair<T, U>;

template<fmt::color TitleStyle, fmt::color BodyStyle>
using Message = FormattedMessage<
  TitleStyle,
  BodyStyle,
  HintStyle<fmt::color::dark_blue, fmt::color::dodger_blue> {},
  WarningStyle<fmt::color::dark_slate_gray, fmt::color::dim_gray> {} >;

template<typename... Args>
auto error(fmt::format_string<Args...> fmt_string, Args&&... args) {
  using Error = Message<fmt::color::dark_red, fmt::color::fire_brick>;

  return Error("Error", fmt::format(fmt_string, std::forward<Args>(args)...));
}

template<typename... Args>
auto warning(fmt::format_string<Args...> fmt_string, Args&&... args) {
  using Warning = Message<fmt::color::chocolate, fmt::color::dark_orange>;

  return Warning("Error", fmt::format(fmt_string, std::forward<Args>(args)...));
}

template<auto TitleStyle, auto BodyStyle, auto HintStyle, auto ContextStyle>
struct MessageWriter<FormattedMessage<TitleStyle, BodyStyle, HintStyle, ContextStyle>, FILE*> {
  using FormattedMsg = FormattedMessage<TitleStyle, BodyStyle, HintStyle, ContextStyle>;

  static auto write(FormattedMsg const& msg, FILE* file) {
    assert((file == stdout or file == stderr) && "The file is expected to be stdout or stderr.");

    print_main_bar(msg.m_title, msg.m_body);

    auto const context_present = not msg.m_context_body.empty();

    if(not msg.hints.empty()) print_hints(context_present, msg.hints);
    if(context_present) print_context(msg.m_context_title, msg.m_context_body);

    newline();
  }

private:
  static auto continued_indent() { fmt::print(" ├"); };

  static auto end_indent() { fmt::print(" └"); };

  static auto newline() { return fmt::print("\n"); };

  static auto format_title(auto const& color, auto const& content) {
    using namespace fmt;

    print(emphasis::bold | fg(color::light_gray) | bg(color), " {} ", content);
  };

  static auto format_content(auto const& color, auto const& content) {
    using namespace fmt;

    print(bg(color) | fg(color::white), " {} ", content);

    newline();
  };

  static auto print_bar(auto const& title_style, auto const& title, auto const& body_style, auto const& body) {
    format_title(title_style, title);
    format_content(body_style, body);
  }

  static auto print_main_bar(auto const& title, auto const& body) {
    print_bar(TitleStyle, title, BodyStyle, body); // main bar
  }

  static auto print_hints(auto context_present, auto const& hints) {
    for(auto i = 0ULL; auto const& [title, body] : hints) {
      if(context_present or i++ == hints.size() - 1)
        continued_indent();
      else end_indent();

      print_bar(HintStyle.title, title, HintStyle.body, body);
    }
  }

  static auto print_context(auto const& title, auto const& body) {
    end_indent();

    format_title(ContextStyle.title, title);

    auto const body_without_newline = [ctx_body = body]() mutable {
      while(ctx_body.back() == '\n')
        ctx_body.pop_back();

      return ctx_body;
    }();

    format_content(ContextStyle.body, body_without_newline);
  }
};

} // namespace thyme
