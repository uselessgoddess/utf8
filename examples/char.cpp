#include <iostream>

#include "utf8.hpp"

using utf8::char_t;

#define OUT(...) std::cout << #__VA_ARGS__ << ": " << (__VA_ARGS__) << "\n"

int main() {
  std::cout << std::boolalpha;

  OUT(char_t('A') == char('A'));
  OUT(char_t("🦀") != char('C'));

  // wide characters from string literal
  char_t ch = "\uD7FF";
  // incorrect: static assertion
  /*
    char_t ch = "";
    char_t ch = "lol";
    char_t ch = "\uDDFF";
  */
  OUT(ch);
}
