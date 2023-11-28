#include <gtest/gtest.h>

#include <iostream>
#include <utf8.hpp>

using namespace utf8::literals;
using utf8::char_t;

TEST(operators, plus) {
  auto str = utf8::string("/* ") + "comment" + " */";
  auto ch = char_t("\u0306");

  ASSERT_EQ(str + ch + ch + ch, "/* comment */̆̆̆"sv);
}

TEST(operators, plus_assign) {
  auto str = utf8::string("/* ");
  char_t ch = u'\u0306';

  str += "comment";
  str += " */";
  str += ch;

  ASSERT_EQ(str + ch + ch, "/* comment */̆̆̆"sv);
}
