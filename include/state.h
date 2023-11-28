#pragma once

namespace utf8 {

/// place like `std::mbstate_t` for using `in-place` style
struct state_t : private std::array<char, 4> {
  constexpr state_t() noexcept = default;

  constexpr operator std::span<char>() noexcept {
    return {this->begin(), this->end()};
  }
};

}  // namespace utf8