#include <iostream>

#include "utf8.hpp"

int main() {
  utf8::string_view str = "𝙷𝚎𝚕𝚕𝚘 𝚆𝚘𝚛𝚕𝚍!";
  std::cout << str << " -- " << str.size() << " bytes\n";
}
