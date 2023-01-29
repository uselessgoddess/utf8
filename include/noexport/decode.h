#pragma once

constexpr auto CONT_MASK = char8_t(0b0011'1111);

namespace utf8::noexport {
constexpr auto first_byte(char8_t byte, std::uint32_t width) noexcept -> std::uint32_t {
  return (byte & (0x7F >> width));
}

constexpr auto acc_cont_byte(std::uint32_t ch, char8_t byte) -> std::uint32_t {
  return (ch << 6) | (byte & CONT_MASK);
}

// size of encoded value from unicode header
constexpr auto thin_decode(char8_t x) noexcept -> size_t {
  return x < 128 ? 1 : x < 0xF0 ? (x < 0xE0 ? 2 : 3) : 4;
}

// size of encoded value from unicode header

// real decode
// todo: maybe receive mut reference to pointer???
constexpr auto decode_char(const char* place) noexcept -> std::uint32_t {
  char8_t x = *place++;
  if (x < 128) {
    return x;
  }

  auto init = first_byte(x, 2);
  auto y = *place++;

  auto ch = acc_cont_byte(init, y);

  if (x >= 0xE0) {
    auto z = *place++;
    auto y_z = acc_cont_byte(y & CONT_MASK, z);
    ch = init << 12 | y_z;

    if (x >= 0xF0) {
      auto w = *place++;
      ch = (init & 0b0000'0111) << 18 | acc_cont_byte(y_z, w);
    }
  }

  return ch;
}

constexpr auto is_cont_byte(char8_t byte) -> bool {
  return std::int8_t(byte) < -64;
}

// size of encoded value from unicode header
constexpr auto seek_decode_back(const char* place) noexcept -> size_t {
  if (char8_t(*--place) < 128) {
    return 1;
  }

  if (is_cont_byte(*--place)) {
    if (is_cont_byte(*--place)) {
      return 4;
    }
    return 3;
  }
  return 2;
}

// todo: maybe receive mut reference to pointer???
constexpr auto decode_char_back(const char* place) noexcept -> std::uint32_t {
  char8_t w = *--place;
  if (w < 128) {
    return w;
  }

  auto z = *--place;
  auto ch = first_byte(z, 2);
  if (is_cont_byte(z)) {
    auto y = *--place;
    ch = first_byte(y, 3);

    if (is_cont_byte(y)) {
      auto x = *--place;
      ch = first_byte(x, 4);
      ch = acc_cont_byte(ch, y);
    }
    ch = acc_cont_byte(ch, z);
  }
  ch = acc_cont_byte(ch, w);

  return ch;
}

}  // namespace utf8::noexport