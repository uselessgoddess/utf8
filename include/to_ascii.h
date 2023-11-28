namespace utf8::noexport {

constexpr char8_t ASCII_CASE_MASK = 0b0010'0000;  // 32

constexpr auto ascii_change_case(char8_t ch) noexcept -> char8_t {
  return ch ^ ASCII_CASE_MASK;
}

constexpr void do_convert(std::span<char> bytes, auto&& convert) noexcept {
  for (auto& byte : bytes) {
    byte = char(convert(byte));
  }
}

}  // namespace utf8::noexport

namespace utf8::ascii {

constexpr auto to_upper(char_t ch) noexcept -> char_t {
  if (ascii::is_lower(ch)) {
    return noexport::ascii_change_case(char8_t(ch));
  } else {
    return ch;
  }
}

constexpr auto to_lower(char_t ch) noexcept -> char_t {
  if (ascii::is_upper(ch)) {
    return noexport::ascii_change_case(char8_t(ch));
  } else {
    return ch;
  }
}

constexpr void do_upper(str_mut str) noexcept {
  noexport::do_convert(str.bytes_mut(noexport::unsafe), to_upper);
}

constexpr void do_lower(str_mut str) noexcept {
  noexport::do_convert(str.bytes_mut(noexport::unsafe), to_lower);
}

}  // namespace utf8::ascii