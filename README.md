# Strongly UTF-8 in C++

This library provide analogues of `std::string_view` and `std::string` (plus safe mutable `string_view` - `str_mut`) \
Strings are always valid UTF-8 (in another cases it is undefined behaviour)

## Basic Usage

`string_view` is implicitly constructible from string literal:

```c++
utf8::string_view hello = "Hello, World!";
```

Invalid UTF-8 in literals will not allow compiling:

```c++
utf8::string_view hello = "Hello, \xff!"; // compile error
```

This library also provide `literals` for `string_view`:

```c++
auto _1 = ""sv;
auto _2 = ""u;
auto _3 = ""U;
```

## Representation

Strings are always valid UTF-8 bytes sequences. Because UTF-8 is a variable width encoding, Strings are typically
smaller than an array of the same chars \
However, because of this it becomes impossible to access the character in a constant time:

```c++
utf8::string_view s = "hello";
ASSERT_EQ(5, s.size());

auto s = std::vector<utf8::char_t>{ 'h', 'e', 'l', 'l', 'o' };
ASSERT_EQ(20, s.size() * sizeof(utf8::char_t));

utf8::string_view s = "💖💖💖💖💖";
ASSERT_EQ(20, s.size());
```

```c++
utf8::string_view  s = "hello";
ASSERT_EQ('l', *std::next(s.chars(), 2))

utf8::string_view  s = "💖💖💖💖💖";
ASSERT_EQ(char_t("💖"), *std::next(s.chars(), 2))
```

Note: the width of 💖 is larger than `char`, so we must create such a character from the string literal that
has `lit.chars()` equal to one `char_t(" ")`.

You can use `.substr`, which ensures that the passed value is a char boundary:

```c++
utf8::string_view s = "hello";
ASSERT_EQ("hell", s.substr(0, 4));

utf8::string_view  s = "💖💖💖💖💖";
ASSERT_EQ("💖", s.substr(0, 4));
```

## About types

### `char_t` - a new character type.

A char is a `Unicode code point`.
This has a fixed numerical definition: code points are in the range 0 to 0x10FFFF, inclusive.

Any value of `char` character is valid unicode point:

```c++
char_t array[] = { '1', 'b', '!', char(255) };
```

No `char_t` may be constructed, whether as a literal or at runtime, that is not a Unicode scalar value:

```c++
char_t(""); // compile error
char_t("hello"); // compile error
char_t("\uDFFF"); // compile error
char_t("💖"); // ok
```

`char_t` is compatible with `char`

```c++
bool is_ascii(char_t ch) {
    return ch <= 0x7F;
}
```

#### Representation

`char_t` is a new-type of `std::uint32_t`:

```c++
static_assert(sizeof(char_t) == sizeof(std::uint32_t));
static_assert(alignof(char_t) == alignof(std::uint32_t));
```

As always, remember that human intuition about "character" may not match Unicode definitions. \
For example, despite appearances, the symbol 'é' is one point in the Unicode code, while 'é' is two points in the
Unicode code:

```c++
auto chars = utf8::string_view("é").chars();
auto iter = chars.begin();
// U+00e9: 'latin small letter e with acute'
ASSERT_EQ(char_t("\u00e9"), *iter++);

auto chars = utf8::string_view("é").chars();
auto iter = chars.begin();
// U+0065: 'latin small letter e'
ASSERT_EQ(char_t("\u0065"), *iter++);
// U+0301: 'combining acute accent'
ASSERT_EQ(char_t("\u0301"), *iter++);
```

```c++
//   in 'constexpr' expansion of 'char_t("e\37777777714\37777777601")'
// error: expression '<throw-expression>' is not a constant expression
//       |     throw "`char_t` from character literal may only contain one codepoint";
auto c = char_t("é");
```

### `string_view` + `string` == `char_t("❤")`

`utf8::string` is a strongly unicode `std::string` \
`utf8::string_string` is any borrowed strongly unicode char sequence (like `std::string_view`)

Use `string_view` to pass strings to functions for read only (does not use `const string&`):

```c++
bool is_ascii(utf8::string_view str) {
    auto chars = str.chars();
    return std::distance(chars.begin(), chars.end()) == str.size();
}

...

is_ascii("héllo"); // ok
is_ascii("h\xffllo"); // compile error
is_ascii(utf8::string()) // explicit compile error: `string_view` of temporary value 

auto s = utf8::string("hello");
is_ascii(s); // ok
```

#### Access

Use some `views` on a string: `.chars()`, `.bytes()`:

```c++
for (char_t ch : utf8::string_view("h🔥llo").chars()) {
    std::cout << "`" << ch << "` ";
} // `h` `🔥` `l` `l` `o`
```

```c++
std::cout << std::hex;
for (char8_t ch : utf8::string_view("h🔥llo").bytes()) {
    if (utf8::is_ascii(ch)) { // or ch <= 0x7f 
        std::cout << "`" << char(ch) << "` ";
    } else {
        std::cout << size_t(ch) << " ";
    }
} // `h` f0 9f 94 a5 `l` `l` `o`
```

`substr` work as std analogue, but check utf-8 boundaries:

```c++
utf8::string_view a = "🗻∈🌏";
utf8::string b = "🌏∈🗻";

ASSERT_EQ(a.substr(0, 4), b.substr(11));
a.substr(1); // error
a.substr(0, 8); // error
a.substr(0, 35); // error
```

All methods from std are preserved as far as possible, and all read only methods in `string` delegated
from `string_view`

### `str_mut` - a way of safety mutable `string_view`
Any string that is not a constant (like a string literal) can be passed as `str_mut`.
```c++
utf8::string place = "С++∈🌏"sv;  // mutable-friendly string
utf8::string_view str = place; // `str_mut` should be used only to pass mutable strings to functions

// unsafe access to unicode bytes representation
// `С` took 2 bytes - https://unicode-table.com/0421/
auto bytes = str.data_mut(); // .bytes_mut(utf8::unsafe).data() as possible
{
  bytes[0] = 0xF0;
  bytes[1] = 0x9F;
  bytes[2] = 0xA6;
  bytes[3] = 0x80;
}

ASSERT_EQ("🦀∈🌏"sv, str);
```

### `utf8::unsafe_t` - You can break Unicode, so do it nicely 
All functions accepting `utf8::unsafe_t` can break utf-8, but will not know it. 
There are no intermediate checks here, so this can lead to undefined behavior

##### Typical usages: 
`utf8::string_view` unchecked from `std::string_view`:
```c++
auto str = utf8::string_view(utf8::unsafe, "hello"); 
auto str = utf8::string_view(utf8::unsafe, "\xff"); // undefined behavior
```
`utf8::char_t` unchecked from `std::uint32_t`
```c++
auto ch = utf8::char_t(utf8::unsafe, 0x2764); // '❤' 
auto ch = utf8::char_t(utf8::unsafe, 0x110000); // undefined behavior
```
`utf8::str_mut` to mutable bytes (`.bytes_mut`):
```c++
auto str = utf8::string("hello");
auto bytes = str.bytes_mut(utf8::unsafe);

bytes.data()[1] = '\0';
bytes.data()[4] = '0';

// utf-8 strings and string_views are not null-terminated
ASSERT_EQ("h\0ll0", str);
```

### `utf8::parse`, `utf8::parse_lossy` - runtime unicode validation

Example from `examples/parse.cpp`
```c++
auto c_str = "H\xffl\xff\xff W\xff\xffrl\xf0\x9f!";

try {
  utf8::parse(c_str);
} catch (utf8::utf8_error& err) {
  auto [valid_to, error_len] = err;

  std::cout << "Error: `" << err.what() << "`\n";
  std::cout << "Valid up to: " << valid_to << " error len: " << error_len.value() << "\n";
}

// `H�l�� W��rl�!`
std::cout << "Lossy: `" << utf8::parse_lossy(c_str) << "`\n";
```