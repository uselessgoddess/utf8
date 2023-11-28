#pragma once

#include "conversions.h"

namespace utf8 {

namespace noexport {
constexpr auto convert_while_ascii(std::string_view str, std::invocable<char> auto&& convert)
    -> std::string {
  std::string place;
  place.reserve(str.size());

  size_t count = 0;
  for (auto byte : str) {
    if (utf8::is_ascii(byte)) {
      place.push_back(static_cast<char>(convert(byte)));
    } else {
      break;
    }
    count++;
  }
  place.resize(count);

  return place;
}

}  // namespace noexport

constexpr auto to_lower(string_view str) -> string {
  auto place = noexport::convert_while_ascii(str, ascii::to_lower);

  auto tail = str.substr(place.size());
  auto out = string(noexport::unsafe, std::move(place));

  for (char_t ch : tail.chars()) {
    for (auto c : noexport::conversions::to_lower(ch)) {
      out.push_back(c);
    }
  }
  return out;
}

constexpr auto to_upper(string_view str) -> string {
  auto place = noexport::convert_while_ascii(str, ascii::to_upper);

  auto tail = str.substr(place.size());
  auto out = string(noexport::unsafe, std::move(place));

  for (char_t ch : tail.chars()) {
    for (auto c : noexport::conversions::to_upper(ch)) {
      out.push_back(c);
    }
  }
  return out;
}

using noexport::conversions::str_t;

constexpr auto to_lower(char_t ch) -> str_t {
  return noexport::conversions::to_lower(ch);
}

constexpr auto to_upper(char_t ch) -> str_t {
  return noexport::conversions::to_upper(ch);
}

}  // namespace utf8
