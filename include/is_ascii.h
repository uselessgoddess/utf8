namespace utf8 {

[[nodiscard]] constexpr auto is_ascii(char_t ch) noexcept -> bool {
  return std::uint32_t(ch) <= 0x7f;
}

[[nodiscard]] constexpr auto is_digit(char_t ch) noexcept -> bool {
  return (ch >= '0' && ch <= '9');
}

[[nodiscard]] constexpr auto is_hexdigit(char_t ch) noexcept -> bool {
  return is_digit(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
}

namespace ascii {

[[nodiscard]] constexpr auto is_alpha(char_t ch) noexcept -> bool {
  return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

[[nodiscard]] constexpr auto is_lower(char_t ch) noexcept -> bool {
  return ch >= 'a' && ch <= 'z';
}

[[nodiscard]] constexpr auto is_upper(char_t ch) noexcept -> bool {
  return ch >= 'A' && ch <= 'Z';
}

[[nodiscard]] constexpr auto is_alnum(char_t ch) noexcept -> bool {
  return is_alpha(ch) || is_digit(ch);
}

// is a `iscntrl` from std
[[nodiscard]] constexpr auto is_control(char_t ch) noexcept -> bool {
  return ch == '\x7F' || (ch >= '\0' && ch <= '\x1F');
}

[[nodiscard]] constexpr auto is_graph(char_t ch) noexcept -> bool {
  return ch >= '!' && ch <= '~';
}

[[nodiscard]] constexpr auto is_punct(char_t ch) noexcept -> bool {
  return (ch >= '!' && ch <= '/') || (ch >= ':' && ch <= '@') || (ch >= '[' && ch <= '`') ||
         (ch >= '{' && ch <= '~');
}

[[nodiscard]] constexpr auto is_space(char_t ch) noexcept -> bool {
  return ch == '\t' || ch == '\n' || ch == '\x0C' || ch == '\r' || ch == ' ';
}

}  // namespace ascii

}  // namespace utf8