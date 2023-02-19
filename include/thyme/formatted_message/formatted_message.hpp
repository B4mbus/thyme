#pragma once

#include <utility>
#include <string>
#include <vector>

#include <fmt/format.h>
#include <fmt/color.h>

#include "thyme/formatted_message/message_writer.hpp"

namespace thyme {

template<
  auto TitleStyle,
  auto BodyStyle,
  auto HintStyle,
  auto ContextStyle >
class FormattedMessage {
public:
  template<typename... Args>
  FormattedMessage(std::string title, std::string desc)
    : m_title(std::move(title)),
      m_body(std::move(desc)) {}

  template<typename... Args>
  auto hint(fmt::format_string<Args...> fmt_string, Args&&... args) {
    return hint_custom_title("Hint", fmt_string, std::forward<Args>(args)...);
  }

  template<typename... Args>
  auto hint_custom_title(std::string title, fmt::format_string<Args...> fmt_string, Args&&... args) {
    hints.emplace_back(
      std::move(title),
      fmt::format(fmt_string, std::forward<Args>(args)...)
    );

    return *this;
  }

  template<typename... Args>
  auto context(std::string title, fmt::format_string<Args...> fmt_string, Args&&... args) {
    m_context_title = std::move(title);
    m_context_body = fmt::format(fmt_string, std::forward<Args>(args)...);
    return *this;
  }

  template<typename First, typename... Rest>
  auto write(First&& first_out, Rest&&... rest_out) const {
    MessageWriter<std::remove_cvref_t<decltype(*this)>, std::remove_cvref_t<First>>::write(*this, std::forward<First>(first_out));

    (MessageWriter<std::remove_cvref_t<decltype(*this)>, std::remove_cvref_t<Rest>>::write(*this, std::forward<Rest>(rest_out)), ...);
  }

private:
  template<typename T, typename U>
  friend class MessageWriter;

  std::string m_title;
  std::string m_body;

  std::vector<std::pair<std::string, std::string>> hints;

  std::string m_context_title = "Context";
  std::string m_context_body;
};

} // namespace thyme
