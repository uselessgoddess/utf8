#pragma once

#include <concepts>

namespace utf8::noexport {
struct unsafe_t {};
constexpr auto unsafe = unsafe_t{};
}  // namespace utf8::noexport

namespace utf8 {
struct mut {};
struct immut {};

template <typename T>
concept qualifier = std::same_as<T, mut> || std::same_as<T, immut>;
}  // namespace utf8
