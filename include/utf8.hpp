#pragma once

#include <bit>
#include <cstdint>
#include <limits>

#ifndef _UTF8_RISK_ENABLE

static_assert(CHAR_BIT == 8, "`char` is not a valid UTF-8 point");
static_assert(std::numeric_limits<char8_t>::max() == (char8_t)char(-1),
              "`char` is not a valid UTF-8 point");
#endif

// clang-format off
#include "noexport/utils.h"
#include "noexport/encode.h"
#include "noexport/validation.h"

#include "basic_string_view.h"
#include "char_t.h"

#include "noexport/decode.h"

#include "inner.h"
#include "basic_string.h"

#include "is_ascii.h"
#include "is_utf8.h"
// clang-format on

namespace utf8 {
using unsafe_t = noexport::unsafe_t;
constexpr auto unsafe = unsafe_t{};
}  // namespace utf8

namespace utf8::noexport {
template <typename>
constexpr bool always_false = false;

template <typename utf8>
constexpr auto utf8_assertion() {
  static_assert(always_false<utf8>,
                "`UTF-8 string is not an iterator\n"
                " try calling `.chars()`, `.bytes()`, `.bytes_mut(unsafe)`");
}

template <typename U, typename T = std::decay_t<U>>
concept utf8_str = std::same_as<T, utf8::string> || std::same_as<T, utf8::string_view> ||
                   std::same_as<T, utf8::str_mut>;
}  // namespace utf8::noexport

namespace std {
constexpr auto begin(utf8::noexport::utf8_str auto&& str) {
  utf8::noexport::utf8_assertion<decltype(str)>();
}

constexpr auto end(utf8::noexport::utf8_str auto&& str) {
  utf8::noexport::utf8_assertion<decltype(str)>();
}
}  // namespace std

#undef mutable
#undef immutable
