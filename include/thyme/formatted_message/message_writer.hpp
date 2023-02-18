#pragma once

namespace thyme {

template<typename FormattedMessage, typename T>
class MessageWriter {
public:
  MessageWriter() = delete;

  static auto write(auto const&...) {
    static_assert(No_MessageWrite_Specialization_Found<T>, "No default writer is defined for MessageWriter.");
  };

private:

  template<typename>
  inline static auto No_MessageWrite_Specialization_Found = false;
};

} // namespace thyme
