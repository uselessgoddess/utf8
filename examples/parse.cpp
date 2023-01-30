#include <iostream>

#include "utf8.hpp"

auto main() -> int {
  auto c_str = "H\xffl\xff\xff W\xff\xffrl\xf0\x9f!";

  try {
    utf8::parse(c_str);
  } catch (utf8::utf8_error& err) {
    auto [valid_to, error_len] = err;

    std::cout << "Error: `" << err.what() << "`\n";
    std::cout << "Valid up to: " << valid_to << " error len: " << error_len.value() << "\n";
  }

  std::cout << "Lossy: `" << utf8::parse_lossy(c_str) << "`\n";
}
