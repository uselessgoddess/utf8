#include <gtest/gtest.h>

#include <algorithm>
#include <iostream>
#include <utf8.hpp>

namespace views = std::views;
namespace ranges = std::ranges;
using utf8::char_t;

TEST(access, push_pop) {
  constexpr utf8::string_view text = R"(
    Bei Nacht im Dorf der Wächter rief: Elfe!
    Ein ganz kleines Elfchen im Walde schlief
    wohl um die Elfe!
    Und meint, es rief ihm aus dem Tal
    bei seinem Namen die Nachtigall,
    oder Silpelit hätt' ihm gerufen.
    Reibt sich der Elf' die Augen aus,
    begibt sich vor sein Schneckenhaus
    und ist als wie ein trunken Mann,
    sein Schläflein war nicht voll getan,
    und humpelt also tippe tapp
    durch’s Haselholz in’s Tal hinab,
    schlupft an der Mauer hin so dicht,
    da sitzt der Glühwurm Licht an Licht.
    Was sind das helle Fensterlein?
    Da drin wird eine Hochzeit sein:
    die Kleinen sitzen bei’m Mahle,
    und treiben’s in dem Saale.
    Da guck' ich wohl ein wenig 'nein!"
    Pfui, stößt den Kopf an harten Stein!
    Elfe, gelt, du hast genug?
    Gukuk!
  )";

  utf8::string str;
  for (auto ch : text.chars()) {
    str.push_back(ch);
  }

  ASSERT_EQ(text, str);

  utf8::string new_str;
  std::optional<char_t> ch;
  while ((ch = str.pop_back())) {
    new_str.push_back(*ch);
  }

  ASSERT_TRUE(str.empty());
  ASSERT_TRUE(ranges::equal(text.chars(), new_str.chars() | views::reverse));
}

TEST(access, starts_ends_with) {
  constexpr utf8::string_view str = "Glühwurm 😋";

  ASSERT_TRUE(str.starts_with("Glühwurm"));
  ASSERT_FALSE(str.starts_with("😋"));

  ASSERT_TRUE(str.ends_with("😋"));
  ASSERT_TRUE(str.ends_with(char_t("😋")));
  ASSERT_FALSE(str.ends_with("\0"));
}

TEST(access, find_first_last) {
  constexpr auto N = std::string_view::npos;

  // projection from cppreference
  // clang-format off
  static_assert(
      1 == utf8::string_view("alignas").find_first_of("klmn") &&
      //                       └────────────────────────┘
      N == utf8::string_view("alignof").find_first_of("wxyz") &&
      //
      1 == utf8::string_view("co_await").substr(4).find_first_of("cba")
      //                           └────────────────────────────────┘
  );
  // clang-format on

  constexpr utf8::string_view str = "👈👈🏽👈🏽👈🏾";

  auto len = char_t("👈").size_utf8();

  ASSERT_EQ(2 * len, str.find_first_of("🏽🏽🏾"));
  ASSERT_EQ(len, str.substr(len).find_first_of("🏽🏽🏾"));
  ASSERT_EQ(0, str.substr(len).find_first_not_of("🏽🏽🏾"));

}