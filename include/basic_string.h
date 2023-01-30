#pragma once

#include <ranges>
#include <string>

namespace utf8 {

#define FROM_STRING_VIEW($name)                                                                   \
  [[nodiscard]] constexpr decltype(auto) $name(auto&&... args)                                    \
      const noexcept(noexcept(string_view(*this).$name(std::forward<decltype(args)>(args)...))) { \
    return string_view(*this).$name(std::forward<decltype(args)>(args)...);                       \
  }

#define FROM_STR_MUT($name)                                                    \
  [[nodiscard]] constexpr decltype(auto) $name(auto&&... args) noexcept(       \
      noexcept(str_mut(*this).$name(std::forward<decltype(args)>(args)...))) { \
    return str_mut(*this).$name(std::forward<decltype(args)>(args)...);        \
  }

template <typename Alloc = std::allocator<char>>
struct basic_string {
  using std_string = std::basic_string<char, std::char_traits<char>, Alloc>;

  std_string _inner;

  static constexpr auto npos = string_view::npos;

  constexpr basic_string(basic_string&&) noexcept = default;
  constexpr basic_string(const basic_string&) noexcept = default;
  constexpr basic_string& operator=(basic_string&&) noexcept = default;
  constexpr basic_string& operator=(const basic_string&) noexcept = default;

  constexpr explicit basic_string(Alloc alloc = {}) : _inner(alloc){};
  constexpr basic_string(string_view str, Alloc alloc = {}) : _inner(str, alloc) {}
  constexpr basic_string(noexport::unsafe_t, std::string_view str, Alloc alloc = {})
      : _inner(str, alloc) {}

  template <typename InputIt>
  constexpr basic_string(InputIt iter, InputIt end, Alloc alloc = {}) : _inner(std_string(alloc)) {
    _inner.reserve(std::distance(iter, end));  // size hint

    for (; iter != end; iter++) {
      push_back(*iter);
    }
  }

  constexpr operator std_string() const noexcept {
    return _inner;
  }

  constexpr operator std::string_view() const noexcept {
    return std::string_view(_inner);
  }

  constexpr operator string_view() const noexcept {
    return {noexport::unsafe, std::string_view(_inner)};
  }

  constexpr operator str_mut() noexcept {
    return {noexport::unsafe, std::string_view(_inner)};
  }

  FROM_STR_MUT(data_mut)
  FROM_STRING_VIEW(data)

  FROM_STRING_VIEW(empty)
  FROM_STRING_VIEW(size)
  FROM_STRING_VIEW(length)
  FROM_STRING_VIEW(max_size)
  FROM_STRING_VIEW(find)
  FROM_STRING_VIEW(rfind)
  FROM_STRING_VIEW(is_boundary)
  FROM_STRING_VIEW(substr)
  FROM_STRING_VIEW(chars)
  FROM_STRING_VIEW(char_indices)
  FROM_STRING_VIEW(bytes)
  FROM_STRING_VIEW(find_first_of)
  FROM_STRING_VIEW(find_first_not_of)
  FROM_STRING_VIEW(find_last_of)
  FROM_STRING_VIEW(find_last_not_of)
  FROM_STRING_VIEW(starts_with)
  FROM_STRING_VIEW(ends_with)

  [[nodiscard("iterators are lazy")]] constexpr auto bytes_mut(noexport::unsafe_t unsafe) noexcept
      -> bytes_mut_view {
    return str_mut(*this).bytes_mut(unsafe);
  }

  // Capacity
  constexpr void reserve(size_t cap) {
    _inner.reserve(cap);
  }

  [[nodiscard]] constexpr auto capacity() const noexcept -> size_t {
    return _inner.capacity();
  }

  constexpr void shrink_to_fit() {
    return _inner.shrink_to_fit();
  }

  // Operations

  constexpr void clear() {
    return _inner.clear();
  }

  constexpr auto _assert_boundary(size_t index) const {
    if (not is_boundary(index)) {
      throw std::out_of_range(
          (std::stringstream() << "index `" << index << "` is not UTF-8 char header").str());
    }
  }

  constexpr auto insert(size_t index, char_t ch) {
    _assert_boundary(index);

    char place[4];
    auto encoded = ch.encode_utf8(std::span(place));
    _inner.insert(index, encoded.data(), encoded.size());
  }

  constexpr auto insert(size_t index, string_view str) {
    _assert_boundary(index);

    auto bytes = str.bytes();
    _inner.insert(_inner.begin() + index, bytes.begin(), bytes.end());
  }

  constexpr auto erase(size_t index) {
    if (auto chars = substr(index).chars(); chars.begin() != chars.end()) {
      auto ch = *chars.begin();
      auto iter = _inner.begin() + index;
      _inner.erase(iter, iter + ch.size_utf8());
    } else {
      throw std::out_of_range("cannot remove a char from the end of a string");
    }
  }

  constexpr auto erase(size_t pos = 0, size_t _count = npos) {
    auto count = std::min(_count, size() - pos);

    _assert_boundary(pos);
    _assert_boundary(pos + count);

    _inner.erase(pos, count);
  }

  constexpr void push_back(char_t ch) noexcept {
    reserve(ch.size_utf8());

    char place[4];
    auto str = ch.encode_utf8(std::span(place));
    for (char b : str.bytes()) {
      _inner.push_back(b);
    }
  }

  constexpr auto pop_back() noexcept -> std::optional<char_t> {
    if (auto chars = this->chars() | std::views::reverse; chars.begin() != chars.end()) {
      auto ch = *chars.begin();
      auto n = ch.size_utf8();
      while (n--) {
        _inner.pop_back();
      }
      return ch;
    } else {
      return std::nullopt;
    }
  }

  constexpr auto append(char_t ch) noexcept -> basic_string& {
    push_back(ch);
    return *this;
  }

  constexpr auto append(string_view str) noexcept -> basic_string& {
    _inner.append(str);
    return *this;
  }

  constexpr auto operator+=(char_t ch) noexcept -> basic_string& {
    return append(ch);
  }

  constexpr auto operator+=(string_view str) noexcept -> basic_string& {
    return append(str);
  }

  //// clang-format off
 private:
  // using base::assign;

  //// Element access
  // using base::at;
  // using base::operator[];
  // using base::back;
  // using base::front;
  // using base::operator std::string_view;

  //// Iterators

  // using base::begin; using base::end;
  // using base::rbegin; using base::rend;
  // using base::cbegin; using base::cend;
  // using base::crbegin; using base::crend;

  //// Operations
  // using base::insert;
  // using base::erase;
  // using base::push_back;
  // using base::append;
  // using base::operator+=;
  // using base::compare;
  // using base::starts_with;
  // using base::ends_with;
  // using base::contains;
  // using base::replace;
  // using base::substr;
  // using base::copy;
  // using base::resize;

  //// Search
  // using base::find;
  // using base::rfind;
  // using base::find_first_of;
  // using base::find_first_not_of;
  // using base::find_last_of;
  // using base::find_last_not_of;
  //// clang-format on
};

template <typename Alloc>
auto operator+(basic_string<Alloc> string, char_t ch) {
  return std::move(string.append(ch));
}

template <typename Alloc>
auto operator+(basic_string<Alloc> string, string_view other) {
  return std::move(string.append(other));
}

using string = basic_string<>;

constexpr auto parse(std::string_view str) -> string_view {
  if (auto error = validate(str)) {
    throw *error;
  } else {
    return {noexport::unsafe, str};
  }
}

constexpr auto parse(noexport::unsafe_t unsafe, std::string_view str) -> string_view {
  return {unsafe, str};
}

constexpr auto parse_lossy(std::string_view str) -> string {
  string place;

  while (true) {
    // fixme: extract to char or string_view constant
    constexpr string_view replacement = "\uFFFD";

    if (auto error = validate(str)) {
      auto [valid_to, error_len] = *error;
      auto [valid, tail] = std::pair{str.substr(0, valid_to), str.substr(valid_to)};

      place.append(string_view(noexport::unsafe, valid));
      place.append(replacement);

      if (error_len) {
        str = tail.substr(*error_len);
      } else {
        return place;
      }
    } else {
      place.append(string_view(noexport::unsafe, str));
      return place;
    }
  }
  // unreachable
}

}  // namespace utf8

template <typename Alloc>
struct std::hash<utf8::basic_string<Alloc>> {
  auto operator()(const utf8::basic_string<Alloc>& str) const noexcept {
    return std::hash<typename utf8::basic_string<Alloc>::std_string>{}(str);
  }
};
