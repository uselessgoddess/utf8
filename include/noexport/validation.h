#pragma once

#include <optional>

namespace utf8 {

constexpr auto char_width(char8_t ch) -> size_t {
  constexpr auto CHAR_WIDTH = std::to_array<std::uint8_t>({
      // 1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 1
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 2
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 3
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 4
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 5
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 6
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 7
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 8
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 9
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // A
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // B
      0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // C
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // D
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  // E
      4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // F
  });
  return CHAR_WIDTH[ch];
}

// repr of `utf8_error`
struct literal_error {
  size_t valid_to;
  std::optional<size_t> error_len;
};

struct utf8_error : std::exception {
  size_t valid_to;
  std::optional<size_t> error_len;

  utf8_error(literal_error error) noexcept /* for beaut fmt */
      : valid_to(error.valid_to), error_len(error.error_len) {}

  utf8_error(size_t valid_to, std::optional<size_t> error_len) noexcept
      : valid_to(valid_to), error_len(error_len) {}

  [[nodiscard]] auto what() const noexcept -> const char* override {
    return "invalid UTF-8, catch as `utf8_error` for more details";
  }
};

constexpr auto validate(std::string_view str) noexcept -> std::optional<literal_error> {
  using some = std::optional<size_t>;

  auto idx = str.begin();
  while (idx != str.end()) {
    auto $old_offset = size_t(idx - str.begin());

#define ERROR($error_len) return std::optional(literal_error{$old_offset, $error_len})
#define NEXT_IN($var)     \
  idx++;                  \
  if (idx >= str.end()) { \
    ERROR(std::nullopt);  \
  }                       \
  $var = *idx
#define NEXT($var) \
  char8_t $var;    \
  NEXT_IN($var)

    char8_t first = *idx;
    if (first >= 128) {
      switch (char_width(first)) {
        case 2: {
          NEXT(next);
          if (int8_t(next) >= -64) {
            ERROR(some(1));
          }
          break;
        }
        case 3: {
          NEXT(next);
          if ((first == 0xE0 && next >= 0xA0 && next <= 0xBF) ||
              (first == 0xED && next >= 0x80 && next <= 0x9F) ||
              (first >= 0xE1 && first <= 0xEC && next >= 0x80 && next <= 0xBF) ||
              (first >= 0xEE && first <= 0xEF && next >= 0xA0 && next <= 0xBF)) {
          } else {
            ERROR(some(1));
          }
          NEXT_IN(next);
          if (int8_t(next) >= -64) {
            ERROR(some(2));
          }
        } break;
        case 4: {
          NEXT(next);
          if ((first == 0xF0 && next >= 0x90 && next <= 0xBF) ||
              (first >= 0xF1 && first <= 0xF3 && next >= 0x80 && next <= 0xBF) ||
              (first == 0xF4 && next >= 0x80 && next <= 0x8F)) {
          } else {
            ERROR(some(2));
          }
          NEXT_IN(next);
          if (int8_t(next) >= -64) {
            ERROR(some(2));
          }
          NEXT_IN(next);
          if (int8_t(next) >= -64) {
            ERROR(some(3));
          }
        } break;
        default:
          ERROR(some(1));
      }
      idx++;
    } else {
      // todo: skip more ascii characters per iter
      idx++;
    }
  }
  return std::nullopt;
}
#undef ERROR

consteval void const_validate(std::string_view str) {
  auto idx = str.begin();
  while (idx != str.end()) {
    char8_t first = *idx;
    if (first >= 128) {
      switch (char_width(first)) {
        case 2: {
          if (int8_t(*++idx) >= -64) {
            throw "invalid character in unicode escape";
          }
          break;
        }
        case 3: {
          char8_t next = *++idx;
          if ((first == 0xE0 && next >= 0xA0 && next <= 0xBF) ||
              (first == 0xED && next >= 0x80 && next <= 0x9F) ||
              (first >= 0xE1 && first <= 0xEC && next >= 0x80 && next <= 0xBF) ||
              (first >= 0xEE && first <= 0xEF && next >= 0xA0 && next <= 0xBF)) {
          } else {
            throw "invalid character in unicode escape";
          }
          if (int8_t(*++idx) >= -64) {
            throw "invalid character in unicode escape";
          }
        } break;
        case 4: {
          char8_t next = *++idx;
          if ((first == 0xF0 && next >= 0x90 && next <= 0xBF) ||
              (first >= 0xF1 && first <= 0xF3 && next >= 0x80 && next <= 0xBF) ||
              (first == 0xF4 && next >= 0x80 && next <= 0x8F)) {
          } else {
            throw "invalid character in unicode escape";
          }
          if (int8_t(*++idx) >= -64) {
            throw "invalid character in unicode escape";
          }
          if (int8_t(*++idx) >= -64) {
            throw "invalid character in unicode escape";
          }
        } break;
        default:
          throw "must be a character in the range [\x00-\x7f]";
      }
      idx++;
    } else {
      // todo: skip more ascii characters per iter
      idx++;
    }
  }
}

}  // namespace utf8
