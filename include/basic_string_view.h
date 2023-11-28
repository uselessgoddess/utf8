#pragma once

#include <concepts>
#include <ranges>

// #include "noexport/encode.h"
// #include "noexport/utils.h"
// #include "noexport/validation.h"

namespace utf8 {

struct char_t;

namespace noexport {
constexpr auto is_utf8_bound(char8_t ch) noexcept -> bool {
  // equivalent to: b < 128 || b >= 192
  return int8_t(ch) >= -0x40;
}
}  // namespace noexport

#define mutable requires std::same_as<Q, mut>
#define immutable requires std::same_as<Q, immut>

template <typename Q>
class basic_string_view {
  using string_view = basic_string_view<immut>;
  using str_mut = basic_string_view<immut>;

 public:
  // std::u8string_view inner; -- cannot be because reinterpret_cast in constexpr is incorrect
  std::string_view _inner;

 public:
  static constexpr auto npos = std::string_view::npos;

  constexpr basic_string_view(noexport::unsafe_t, std::string_view str) : _inner(str) {}

  // forbid `use after free`
  // template <typename Alloc>
  // constexpr basic_string_view(basic_string<Alloc>&&) = delete;

  constexpr basic_string_view(noexport::unsafe_t, std::string_view str) immutable : _inner(str) {}

  template <size_t N>
  consteval basic_string_view(const char (&lit)[N]) immutable : _inner(lit, N - 1) {
    const_validate(std::string_view(lit, N - 1));
  }

  constexpr operator basic_string_view<immut>() const noexcept
    requires std::same_as<Q, mut>
  {
    return {noexport::unsafe, _inner};
  }

  constexpr operator std::string_view() const noexcept {
    return _inner;
  }

  constexpr operator std::string() const {
    return std::string(_inner);
  }

  [[nodiscard("iterators are lazy")]] constexpr auto chars() const noexcept;

  [[nodiscard("iterators are lazy")]] constexpr auto char_indices() const noexcept;

  [[nodiscard("iterators are lazy")]] constexpr auto bytes() const noexcept;

  [[nodiscard("iterators are lazy")]] constexpr auto bytes_mut(noexport::unsafe_t) const noexcept
      mutable;

  [[nodiscard]] constexpr auto data() const noexcept {
    return _inner.data();
  }

  [[nodiscard]] constexpr auto data_mut() const noexcept mutable {
    // SAFETY: `str_mut` guaranty refer to mutable friendly data (no stack constants, etc...)
    return const_cast<char*>(_inner.data());
  }

  [[nodiscard]] constexpr auto size() const noexcept {
    return _inner.size();
  }

  [[nodiscard]] constexpr auto length() const noexcept {
    return _inner.length();
  }

  [[nodiscard]] constexpr auto max_size() const noexcept {
    return _inner.max_size();
  }

  [[nodiscard]] constexpr auto empty() const noexcept {
    return _inner.empty();
  }

  [[nodiscard]] constexpr auto is_boundary(size_t index) const noexcept -> bool {
    if (index == 0 || index == size()) {
      return true;
    } else if (index < size()) {
      return noexport::is_utf8_bound(_inner[index]);
    } else {
      return false;
    }
  }

  constexpr auto substr(size_t pos = 0, size_t _n = npos) const noexcept(false)
      -> basic_string_view {
    auto n = std::min(_n, size() - pos);

    auto sv_check = [this](size_t index) {
      if (not is_boundary(index)) {
        throw std::out_of_range(std::format("index `{}` is not UTF-8 character point", index));
      }
    };
    sv_check(pos);
    sv_check(pos + n);

    return {noexport::unsafe, _inner.substr(pos, n)};
  }

  [[nodiscard]] constexpr auto find(char_t ch) const noexcept -> size_t;
  [[nodiscard]] constexpr auto rfind(char_t ch) const noexcept -> size_t;

  [[nodiscard]] constexpr auto find(string_view str) const noexcept -> size_t {
    return _inner.find(str._inner);
  }

  [[nodiscard]] constexpr auto rfind(string_view str) const noexcept -> size_t {
    return _inner.rfind(str._inner);
  }

  [[nodiscard]] constexpr auto find_first_of(string_view str) const noexcept -> size_t;

  [[nodiscard]] constexpr auto find_first_not_of(string_view str) const noexcept -> size_t;

  [[nodiscard]] constexpr auto find_last_of(string_view str) const noexcept -> size_t;

  [[nodiscard]] constexpr auto find_last_not_of(string_view str) const noexcept -> size_t;

  [[nodiscard]] constexpr auto starts_with(char_t ch) const noexcept -> bool;

  [[nodiscard]] constexpr auto starts_with(string_view str) const noexcept -> bool {
    return _inner.starts_with(str);
  }

  [[nodiscard]] constexpr auto ends_with(char_t ch) const noexcept -> bool;

  [[nodiscard]] constexpr auto ends_with(string_view str) const noexcept -> bool {
    return _inner.ends_with(str);
  }

  [[nodiscard]] constexpr auto contains(auto str) const noexcept -> bool {
    return find(str) != npos;
  }
};

using string_view = basic_string_view<immut>;
using str_mut = basic_string_view<mut>;

constexpr auto operator==(string_view a, string_view b) -> bool {
  return std::string_view(a) == std::string_view(b);
}

static auto operator<<(std::ostream& out, string_view str) -> std::ostream& {
  return out << str._inner;
}

// template <typename Q>
// constexpr chars_view::chars_view(basic_string_view<Q> str) noexcept
//     : chars_view(noexport::unsafe, str.inner) {}

// constexpr auto char_t::encode_ascii(std::span<char> dst) const noexcept -> string_view {
//   return noexport::encode_utf8(std::uint32_t(*this), dst);
// }

namespace literals {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wliteral-suffix"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wliteral-suffix"

consteval auto operator"" u(const char* str, size_t len) -> string_view {
  const_validate(str);
  return {noexport::unsafe, std::string_view(str, len)};
}

consteval auto operator"" U(const char* str, size_t len) -> string_view {
  const_validate(str);
  return {noexport::unsafe, std::string_view(str, len)};
}

consteval auto operator""sv(const char* str, size_t len) -> string_view {
  const_validate(str);
  return {noexport::unsafe, std::string_view(str, len)};
}

#pragma GCC diagnostic pop
#pragma clang diagnostic pop
}  // namespace literals
}  // namespace utf8

template <>
struct std::hash<utf8::string_view> {
  auto operator()(utf8::string_view str) const noexcept {
    return std::hash<std::string_view>{}(str);
  }
};
