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
    using namespace fmt;

    auto const format_title = [](auto const& color, auto const& content) {
      print(emphasis::bold | fg(color::light_gray) | bg(color), " {} ", content);
    };

    auto const newline = [] { return print("\n"); };
    auto const format_content = [newline](auto const& color, auto const& content) {
      print(bg(color) | fg(color::white), " {} ", content);
      newline();
    };

    auto const continued_indent = [] { print(" ├"); };
    auto const end_indent = [] { print(" └"); };

    format_title(TitleStyle, msg.m_title);
    format_content(BodyStyle, msg.m_body);

    auto const context_present = not msg.m_context_body.empty();

    if(not msg.hints.empty()) {
      for(auto i = 0ULL; i < msg.hints.size(); ++i) {
        if(context_present or i == msg.hints.size() - 1)
          continued_indent();
        else
          end_indent();

        auto const& [title, body] = msg.hints[i];

        format_title(HintStyle.title, title);
        format_content(HintStyle.body, body);
      }
    }

    if(context_present) {
      end_indent();

      format_title(ContextStyle.title, msg.m_context_title);

      auto const body_without_newline = [ctx_body = msg.m_context_body]() mutable {
        while(ctx_body.back() == '\n')
          ctx_body.pop_back();

        return ctx_body;
      }();

      format_content(ContextStyle.body, body_without_newline);
    }

    return newline();
  }
};

} // namespace thyme
