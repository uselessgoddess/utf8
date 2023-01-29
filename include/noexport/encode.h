#pragma once

#include <cstdint>  // std::uint32_t | size_t
#include <sstream>  // std::stringstream
#include <span>  // std::span

namespace utf8::noexport {

// clang-format off
constexpr auto TAG_CONT  = char(0b1000'0000);
constexpr auto TAG_TWO   = char(0b1100'0000);
constexpr auto TAG_THREE = char(0b1110'0000);
constexpr auto TAG_FOUR  = char(0b1111'0000);
// clang-format on
constexpr std::uint32_t MAX_ONE = 0x80;
constexpr std::uint32_t MAX_TWO = 0x800;
constexpr std::uint32_t MAX_THREE = 0x10000;

constexpr auto size_utf8(std::uint32_t code) noexcept -> size_t {
  if (code < MAX_ONE) {
    return 1;
  } else if (code < MAX_TWO) {
    return 2;
  } else if (code < MAX_THREE) {
    return 3;
  } else {
    return 4;
  }
}

constexpr auto encode_utf8(std::uint32_t code, std::span<char> dst) -> std::span<char> {
  auto len = size_utf8(code);

  if (len > 4 || len > dst.size()) {
    // can be use `.append`, but it too much
    throw std::logic_error((std::stringstream()
                            << "encode_utf8: need " << len << " bytes to encode U+" << std::hex
                            << code << ", but the buffer has " << dst.size())
                               .str());
  }

  switch (len) {
    case 1:
      dst[0] = char(code);
      break;
    case 2:
      dst[0] = char(code >> 6 & 0x1F) | TAG_TWO;
      dst[1] = char(code & 0x3F) | TAG_CONT;
      break;
    case 3:
      dst[0] = char(code >> 12 & 0x0F) | TAG_THREE;
      dst[1] = char(code >> 6 & 0x3F) | TAG_CONT;
      dst[2] = char(code & 0x3F) | TAG_CONT;
      break;
    case 4:
      dst[0] = char(code >> 18 & 0x07) | TAG_FOUR;
      dst[1] = char(code >> 12 & 0x3F) | TAG_CONT;
      dst[2] = char(code >> 6 & 0x3F) | TAG_CONT;
      dst[3] = char(code & 0x3F) | TAG_CONT;
      break;
    default:
      // unreachable
      std::abort();
  }

  return dst.subspan(0, len);
}

constexpr auto is_valid_code(std::uint32_t c) noexcept -> bool {
  return (c ^ 0xD800) - 0x800 < 0x110000 - 0x800;
}

}  // namespace utf8::noexport