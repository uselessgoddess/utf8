#pragma once

#include <bit>
#include <cstdint>  // std::uint32_t
#include <optional>
#include <span>
#include <sstream>

namespace utf8 {

struct char_t {
  // static constexpr auto replacement = char_t("⌛");
  static constexpr auto unicode_version = std::array<std::uint8_t, 3>{15, 0, 0};

  std::uint32_t inner;

  constexpr char_t(char_t&&) = default;
  constexpr char_t(const char_t&) = default;
  constexpr ~char_t() = default;

  consteval char_t(noexport::unsafe_t, std::string_view str);

  // is consteval - allow only string literals
  template <size_t N>
  explicit consteval char_t(const char (&lit)[N])
      : char_t(noexport::unsafe, std::string_view(lit)) {}

  consteval char_t(char16_t ch) : inner(ch) {
    if (not noexport::is_valid_code(ch)) {
      throw "invalid UTF-8 character";
    }
  }

  consteval char_t(char32_t ch) : inner(ch) {
    if (not noexport::is_valid_code(ch)) {
      throw "invalid UTF-8 character";
    }
  }

  constexpr char_t(char c) noexcept : inner(std::uint8_t(c)) {}
  constexpr char_t(char8_t c) noexcept : inner(c) {}

  constexpr char_t& operator=(const char_t&) noexcept = default;

  explicit constexpr char_t(noexport::unsafe_t, std::uint32_t c) : inner(c) {}

  constexpr auto operator==(const char_t&) const noexcept -> bool = default;
  constexpr auto operator<=>(const char_t&) const noexcept = default;

  template <std::integral T>
  explicit constexpr operator T() const noexcept {
    return static_cast<T>(inner);
  }

  constexpr static auto from(std::uint32_t c) noexcept -> std::optional<char_t> {
    if (noexport::is_valid_code(c)) {
      return char_t(noexport::unsafe, c);
    } else {
      return std::nullopt;
    }
  }

  [[nodiscard]] constexpr auto size_utf8() const noexcept -> size_t {
    return noexport::size_utf8(std::uint32_t(*this));
  }

  constexpr auto encode_utf8(std::span<char> dst) const -> str_mut {
    auto out = noexport::encode_utf8(std::uint32_t(*this), dst);
    return {noexport::unsafe, {out.data(), out.size()}};
  }
};

static_assert(sizeof(char_t) == sizeof(std::uint32_t));
static_assert(alignof(char_t) == alignof(std::uint32_t));

static auto operator<<(std::ostream& out, char_t ch) -> std::ostream& {
  char place[4] = {};
  auto span = noexport::encode_utf8(std::uint32_t(ch), std::span(place));
  return out.write(reinterpret_cast<const char*>(span.data()), span.size());
}

namespace literals {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wliteral-suffix"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wliteral-suffix"

consteval auto operator"" c(const char* str, size_t len) -> char_t {
  const_validate(str);
  return {noexport::unsafe, std::string_view(str, len)};
}

#pragma GCC diagnostic pop
#pragma clang diagnostic pop
}  // namespace literals

}  // namespace utf8
