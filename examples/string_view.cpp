#include <iostream>

#include "utf8.hpp"

using utf8::char_t;
using namespace utf8::literals;

int main() {
  std::cout << std::hex;
  utf8::string_view str = "ΠΉΠΎππ½π";
  //utf8::string string = "ΠΉΠΎππ½π"_utf8;

  std::cout << "size: " << str.size() << "\n";
  std::cout << "substr(7, 4):  `" << str.substr(4, 4) << "`\n";
  std::cout << "find(`π½π`): `" << std::dec << str.find("π½π") << std::hex << "\n";
  for (char8_t ch : str.bytes()) {
    std::cout << "x" << size_t(ch) << " ";
  }
  std::cout << "\n";
  for (utf8::char_t ch : str.chars()) {
    std::cout << "`" << ch << "` - u{" << size_t(ch) << "}\n";
  }
  std::cout << "\n";
}
