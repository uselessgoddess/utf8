#include <gtest/gtest.h>

#include <iostream>
#include <utf8.hpp>

using namespace utf8::literals;

TEST(basic, it_works) {
  auto a = utf8::string("2");
  utf8::string b = {"2"};  // initialization doesn't allow implicit casts
  // utf8::string b = "2"sv;

  // gtest coerce const string literal to `const char*`
  // so use explicit `utf8::string_view`
  ASSERT_EQ(a + b, "22"sv);

  if (a + b != "22") {
    FAIL();
  }
}
