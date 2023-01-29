#include <gtest/gtest.h>

#include <algorithm>
#include <iostream>
#include <utf8.hpp>

namespace views = std::views;
namespace ranges = std::ranges;
using utf8::char_t;

TEST(chars, ascii) {
  utf8::string_view word = "goodbye";

  auto all = [](auto&&) {
    return true;
  };
  auto count = ranges::count_if(word.chars(), all);
  ASSERT_EQ(7, count);

  auto chars = word.chars();
  auto iter = chars.begin();

  ASSERT_EQ('g', *iter++);
  ASSERT_EQ('o', *iter++);
  ASSERT_EQ('o', *iter++);
  ASSERT_EQ('d', *iter++);
  ASSERT_EQ('b', *iter++);
  ASSERT_EQ('y', *iter++);
  ASSERT_EQ('e', *iter++);

  ASSERT_EQ(chars.end(), iter);
}

TEST(chars, non_ascii) {
  utf8::string_view y = "y̆";

  auto chars = y.chars();
  auto iter = chars.begin();

  ASSERT_EQ('y', *iter++);  // not 'y̆'
  ASSERT_EQ(char_t("\u0306"), *iter++);

  ASSERT_EQ(chars.end(), iter);
}

TEST(char_indices, ascii) {
  utf8::string_view word = "goodbye";

  auto all = [](auto&&) {
    return true;
  };
  auto count = ranges::count_if(word.chars(), all);
  ASSERT_EQ(7, count);

  auto chars = word.char_indices();
  auto iter = chars.begin();

  using pair = std::pair<size_t, char_t>;

  ASSERT_EQ(pair(0, 'g'), *iter++);
  ASSERT_EQ(pair(1, 'o'), *iter++);
  ASSERT_EQ(pair(2, 'o'), *iter++);
  ASSERT_EQ(pair(3, 'd'), *iter++);
  ASSERT_EQ(pair(4, 'b'), *iter++);
  ASSERT_EQ(pair(5, 'y'), *iter++);
  ASSERT_EQ(pair(6, 'e'), *iter++);

  ASSERT_EQ(chars.end(), iter);
}

TEST(char_indices, non_ascii) {
  utf8::string_view y = "y̆oppa";

  auto chars = y.char_indices();
  auto iter = chars.begin();

  using pair = std::pair<size_t, char_t>;

  ASSERT_EQ(pair(0, 'y'), *iter++);  // not 'y̆'
  ASSERT_EQ(pair(1, char_t("\u0306")), *iter++);

  // last char took up 2 bytes
  ASSERT_EQ(pair(3, 'o'), *iter++);
  ASSERT_EQ(pair(4, 'p'), *iter++);
  ASSERT_EQ(pair(5, 'p'), *iter++);
  ASSERT_EQ(pair(6, 'a'), *iter++);

  ASSERT_EQ(chars.end(), iter);
}

TEST(bytes, basic) {
  utf8::string_view wesker = "uroboros";

  auto bytes = wesker.bytes();
  auto iter = bytes.begin();

  ASSERT_EQ('u', *iter++);
  ASSERT_EQ('r', *iter++);
  ASSERT_EQ('o', *iter++);
  ASSERT_EQ('b', *iter++);
  ASSERT_EQ('o', *iter++);
  ASSERT_EQ('r', *iter++);
  ASSERT_EQ('o', *iter++);
  ASSERT_EQ('s', *iter++);

  ASSERT_EQ(bytes.end(), iter);
}

using namespace utf8::literals;


TEST(bytes, raw_access) {
  utf8::string str = "С++∈🌏"sv;  // mutable-friendly string

  // `С` took 2 bytes - https://unicode-table.com/0421/
  auto bytes = str.bytes_mut(utf8::unsafe).data();
  {
    bytes[0] = 0xF0;
    bytes[1] = 0x9F;
    bytes[2] = 0xA6;
    bytes[3] = 0x80;
  }

  ASSERT_EQ("🦀∈🌏"sv, str);
}