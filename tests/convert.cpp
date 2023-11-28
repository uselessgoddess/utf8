#include <gtest/gtest.h>

#include <algorithm>
#include <iostream>
#include <utf8.hpp>

using namespace utf8::literals;
namespace ascii = utf8::ascii;
using utf8::char_t;

// I want the `quickcheck` in C++

TEST(to_lower, ascii) {
  namespace ascii = utf8::ascii;

  ASSERT_EQ(char_t('g'), ascii::to_lower('G'));
  ASSERT_NE(char_t(u'δ'), ascii::to_lower(u'Δ'));
  ASSERT_EQ(char_t(U'🍌'), ascii::to_lower(U'🍌'));
}

TEST(to_upper, ascii) {
  namespace ascii = utf8::ascii;

  ASSERT_EQ(char_t('G'), ascii::to_upper('g'));
  ASSERT_NE(char_t(u'Δ'), ascii::to_upper(u'δ'));
  ASSERT_EQ(char_t(U'🍌'), ascii::to_upper(U'🍌'));
}

TEST(to_lower, unicode) {
  ASSERT_EQ("δ"sv, utf8::to_lower(u'Δ').str());
  ASSERT_EQ("🍌"sv, utf8::string(utf8::to_upper(U'🍌')));

  ASSERT_EQ("农历新年"sv, utf8::to_lower("农历新年"));
  // ASSERT_EQ("ὀδυσσεύς"sv, utf8::to_lower("ὈΔΥΣΣΕΎΣ")); -- map sigma at the end of word
}

TEST(to_upper, unicode) {
  ASSERT_EQ("Δ"sv, utf8::to_upper(u'δ').str());
  ASSERT_EQ("🍌"sv, utf8::string(utf8::to_upper(U'🍌')));

  ASSERT_EQ("农历新年"sv, utf8::to_upper("农历新年"));
  ASSERT_EQ("TSCHÜSS"sv, utf8::to_upper("tschüß"));
}

TEST(do_lower, ascii) {
  utf8::string str = "g0Оdbye MONkEy 🍌"sv;
  ascii::do_lower(str);

  ASSERT_EQ("g0Оdbye monkey 🍌"sv, str);
}

TEST(do_upper, ascii) {
  utf8::string str = "g0Оdbye MONkEy 🍌"sv;
  ascii::do_upper(str);

  ASSERT_EQ("G0ОDBYE MONKEY 🍌"sv, str);
}
