#pragma once

namespace utf8 {
namespace noexport {
struct char_iter {
  using iterator_concept = std::bidirectional_iterator_tag;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = char_t;
  using difference_type = ptrdiff_t;

  const char* ptr;

  explicit constexpr char_iter() noexcept = default;
  explicit constexpr char_iter(const char* ptr) noexcept : ptr(ptr) {}

  constexpr auto operator++() -> char_iter& {
    ptr += noexport::thin_decode(*ptr);
    return *this;
  }

  constexpr auto operator++(int) -> char_iter {
    auto tmp = *this;
    ++(*this);
    return tmp;
  }

  constexpr auto operator--() -> char_iter& {
    ptr -= noexport::seek_decode_back(ptr);
    return *this;
  }

  constexpr auto operator--(int) -> char_iter {
    auto tmp = *this;
    --(*this);
    return tmp;
  }

  constexpr auto operator*() const -> value_type {
    return char_t(noexport::unsafe, noexport::decode_char(ptr));
  }

  constexpr auto operator==(const char_iter& other) const noexcept -> bool {
    return ptr == other.ptr;
  }
};

struct char_indices_iter {
  using iterator_concept = std::bidirectional_iterator_tag;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = std::pair<size_t, char_t>;
  using difference_type = ptrdiff_t;

  size_t index;
  char_iter iter;

  explicit constexpr char_indices_iter() noexcept = default;
  explicit constexpr char_indices_iter(size_t index, char_iter iter) noexcept
      : index(index), iter(iter) {}

  constexpr auto operator++() -> char_indices_iter& {
    auto prev = iter++;
    index += iter.ptr - prev.ptr;
    return *this;
  }

  constexpr auto operator++(int) -> char_indices_iter {
    auto tmp = *this;
    ++(*this);
    return tmp;
  }

  constexpr auto operator--() -> char_indices_iter& {
    auto cur = iter--;
    index -= cur.ptr - iter.ptr;
    return *this;
  }

  constexpr auto operator--(int) -> char_indices_iter {
    auto tmp = *this;
    --(*this);
    return tmp;
  }

  constexpr auto operator*() const -> value_type {
    return {index, *iter};
  }

  constexpr auto operator==(const char_indices_iter& other) const noexcept -> bool {
    return iter == other.iter;
  }
};
}  // namespace noexport

class chars_view : public std::ranges::view_interface<chars_view> {
  noexport::char_iter _begin{}, _end{};

 public:
  explicit constexpr chars_view(noexport::unsafe_t, std::string_view str) noexcept
      : _begin(str.begin()), _end(str.end()) {}

  template <typename Q>
  explicit constexpr chars_view(basic_string_view<Q> str) noexcept
      : chars_view(noexport::unsafe, str._inner) {}

  [[nodiscard]] constexpr auto begin() const noexcept {
    return _begin;
  }
  [[nodiscard]] constexpr auto end() const noexcept {
    return _end;
  }
};

class char_indices_view : public std::ranges::view_interface<char_indices_view> {
  noexport::char_indices_iter _begin, _end;

 public:
  explicit constexpr char_indices_view(noexport::unsafe_t, chars_view chars, size_t len) noexcept
      : _begin(0, chars.begin()), _end(len, chars.end()) {}

  [[nodiscard]] constexpr auto begin() const noexcept {
    return _begin;
  }
  [[nodiscard]] constexpr auto end() const noexcept {
    return _end;
  }
};

class bytes_view : public std::ranges::view_interface<bytes_view> {
  const char* _begin;
  const char* _end;

 public:
  explicit constexpr bytes_view(std::string_view str) noexcept
      : _begin(str.begin()), _end(str.end()) {}

  [[nodiscard]] constexpr auto begin() const noexcept {
    return _begin;
  }
  [[nodiscard]] constexpr auto end() const noexcept {
    return _end;
  }
};

class bytes_mut_view : public std::ranges::view_interface<bytes_mut_view> {
  char* _begin;
  char* _end;

 public:
  explicit constexpr bytes_mut_view(noexport::unsafe_t, char* begin, char* end) noexcept
      : _begin(begin), _end(end) {}

  [[nodiscard]] constexpr auto begin() const noexcept {
    return _begin;
  }
  [[nodiscard]] constexpr auto end() const noexcept {
    return _end;
  }
};

template <typename Q>
constexpr auto basic_string_view<Q>::chars() const noexcept {
  return chars_view(*this);
}

template <typename Q>
constexpr auto basic_string_view<Q>::char_indices() const noexcept {
  return char_indices_view(noexport::unsafe, chars(), size());
}

template <typename Q>
constexpr auto basic_string_view<Q>::bytes() const noexcept {
  return bytes_view(_inner);
}

template <typename Q>
constexpr auto basic_string_view<Q>::bytes_mut(noexport::unsafe_t) const noexcept mutable {
  // SAFETY: `str` must refer to mutable-friendly data
  return bytes_mut_view(noexport::unsafe, data_mut(), data_mut() + size());
}

consteval char_t::char_t(noexport::unsafe_t, std::string_view raw) {
  auto str = string_view(noexport::unsafe, raw);
  auto chars = str.chars();
  if (auto iter = chars.begin(); std::next(iter) == chars.end()) {
    inner = std::uint32_t(*iter);
  } else {
    throw "`char_t` from character literal may only contain one codepoint";
  }
}

template <typename Q>
constexpr auto basic_string_view<Q>::find(char_t ch) const noexcept -> size_t {
  char place[4] = {};
  auto str = ch.encode_utf8(std::span(place));
  return _inner.find(str);
}

template <typename Q>
constexpr auto basic_string_view<Q>::rfind(char_t ch) const noexcept -> size_t {
  char place[4] = {};
  auto str = ch.encode_utf8(std::span(place));
  return _inner.rfind(str);
}

template <typename Q>
constexpr auto basic_string_view<Q>::find_first_of(string_view str) const noexcept -> size_t {
  for (auto [idx, ch] : char_indices()) {
    if (str.find(ch) != npos) {
      return idx;
    }
  }
  return npos;
}

template <typename Q>
constexpr auto basic_string_view<Q>::find_first_not_of(string_view str) const noexcept -> size_t {
  for (auto [idx, ch] : char_indices()) {
    if (str.find(ch) == npos) {
      return idx;
    }
  }
  return npos;
}

template <typename Q>
constexpr auto basic_string_view<Q>::find_last_of(string_view str) const noexcept -> size_t {
  for (auto [idx, ch] : char_indices() | std::views::reverse) {
    if (str.find(ch) != npos) {
      return idx;
    }
  }
  return npos;
}

template <typename Q>
constexpr auto basic_string_view<Q>::find_last_not_of(string_view str) const noexcept -> size_t {
  for (auto [idx, ch] : char_indices() | std::views::reverse) {
    if (str.find(ch) == npos) {
      return idx;
    }
  }
  return npos;
}

template <typename Q>
constexpr auto basic_string_view<Q>::starts_with(char_t ch) const noexcept -> bool {
  return _inner.starts_with(ch.encode_utf8(utf8::state_t{}));
}

template <typename Q>
constexpr auto basic_string_view<Q>::ends_with(char_t ch) const noexcept -> bool {
  return _inner.ends_with(ch.encode_utf8(utf8::state_t{}));
}

}  // namespace utf8

namespace std::ranges {
template <>
constexpr bool enable_borrowed_range<utf8::chars_view> = true;
template <>
constexpr bool enable_borrowed_range<utf8::char_indices_view> = true;
template <>
constexpr bool enable_borrowed_range<utf8::bytes_view> = true;
template <>
constexpr bool enable_borrowed_range<utf8::bytes_mut_view> = true;
}  // namespace std::ranges
