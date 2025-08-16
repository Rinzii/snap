
#include "snap/fixed_string.hpp"

#include <array>
#include <string>
#include <string_view>
#include <utility>

#if defined(__cpp_lib_containers_ranges) && (__cpp_lib_containers_ranges >= 202202L)
#  define SNAP_HAS_STD_FROM_RANGE 1
#else
#  define SNAP_HAS_STD_FROM_RANGE 0
#endif

#ifndef SNAP_HAS_CONSTEXPR_STRING
#  if defined(__cpp_lib_constexpr_string) && __cpp_lib_constexpr_string >= 201907L
#    define SNAP_HAS_CONSTEXPR_STRING 1
#  else
#    define SNAP_HAS_CONSTEXPR_STRING 0
#  endif
#endif

#ifndef SNAP_HAS_CONSTEXPR_ITERATOR
#  if defined(__cpp_lib_constexpr_iterator) && __cpp_lib_constexpr_iterator >= 201811L
#    define SNAP_HAS_CONSTEXPR_ITERATOR 1
#  else
#    define SNAP_HAS_CONSTEXPR_ITERATOR 0
#  endif
#endif

// test inputs
constexpr std::array<char, 3> arr{{'a','b','c'}};

#if SNAP_HAS_STD_FROM_RANGE
auto from_string = [] {
    std::string txt = "abc";
    return snap::fixed_string<3>(std::from_range, txt);
};
#endif

[[maybe_unused]] auto from_string_iter = [] {
    std::string txt = "abc";
    return snap::fixed_string<3>(txt.begin(), txt.end());
};

// constructions that are constexpr in C++17
constexpr snap::fixed_string<0> txt0{};
constexpr snap::basic_fixed_string txt1('a');
constexpr snap::basic_fixed_string txt2('a','b','c');
constexpr snap::basic_fixed_string txt3 = "abc";
constexpr snap::fixed_string<3>   txt4(arr.begin(), arr.end());

#if SNAP_HAS_STD_FROM_RANGE
constexpr snap::basic_fixed_string txt5(std::from_range, arr);
#endif

// needs constexpr std::string (C++20) and from_range (C++23)
#if SNAP_HAS_STD_FROM_RANGE && SNAP_HAS_CONSTEXPR_STRING
constexpr snap::basic_fixed_string txt6(from_string());
#endif

// needs constexpr std::string (C++20) for iterator construction at CT time
#if SNAP_HAS_CONSTEXPR_STRING
constexpr snap::basic_fixed_string txt7(from_string_iter());
#endif

// pointer iterators are fine in C++17 constexpr; reverse_iterator needs C++20 constexpr iterators
constexpr snap::fixed_string<3> txt8(txt2.begin(), txt2.end());
#if SNAP_HAS_CONSTEXPR_ITERATOR
constexpr snap::fixed_string<3> txt9(txt2.rbegin(), txt2.rend());
#endif

// ---- static assertions ----
static_assert(txt0.size() == 0); // NOLINT
static_assert(txt1.size() == 1); // NOLINT
static_assert(txt2.size() == 3); // NOLINT
static_assert(txt3.size() == 3); // NOLINT
static_assert(txt4.size() == 3); // NOLINT
#if SNAP_HAS_STD_FROM_RANGE
static_assert(txt5.size() == 3); // NOLINT
#endif
#if SNAP_HAS_STD_FROM_RANGE && SNAP_HAS_CONSTEXPR_STRING
static_assert(txt6.size() == 3); // NOLINT
#endif
#if SNAP_HAS_CONSTEXPR_STRING
static_assert(txt7.size() == 3); // NOLINT
#endif
static_assert(txt8.size() == 3); // NOLINT
#if SNAP_HAS_CONSTEXPR_ITERATOR
static_assert(txt9.size() == 3); // NOLINT
#endif

static_assert(txt0.length() == 0); // NOLINT
static_assert(txt1.length() == 1); // NOLINT
static_assert(txt2.length() == 3); // NOLINT

static_assert(txt0.max_size() == 0); // NOLINT
static_assert(txt1.max_size() == 1); // NOLINT
static_assert(txt2.max_size() == 3); // NOLINT

static_assert(txt0.empty()); // NOLINT
static_assert(!txt1.empty()); // NOLINT
static_assert(!txt2.empty()); // NOLINT
static_assert(!txt3.empty()); // NOLINT
static_assert(!txt4.empty()); // NOLINT
#if SNAP_HAS_STD_FROM_RANGE
static_assert(txt5.empty() == false); // NOLINT
#endif
#if SNAP_HAS_STD_FROM_RANGE && SNAP_HAS_CONSTEXPR_STRING
static_assert(txt6.empty() == false); // NOLINT
#endif
#if SNAP_HAS_CONSTEXPR_STRING
static_assert(txt7.empty() == false); // NOLINT
#endif
static_assert(txt8.empty() == false); // NOLINT
#if SNAP_HAS_CONSTEXPR_ITERATOR
static_assert(txt9.empty() == false); // NOLINT
#endif

static_assert(txt1[0] == 'a');
static_assert(txt2[0] == 'a');
static_assert(txt2[1] == 'b');
static_assert(txt2[2] == 'c');
#if SNAP_HAS_CONSTEXPR_ITERATOR
static_assert(txt9[0] == 'c');
static_assert(txt9[1] == 'b');
static_assert(txt9[2] == 'a');
#endif

static_assert(txt1.at(0) == 'a');
static_assert(txt2.at(0) == 'a');
static_assert(txt2.at(1) == 'b');
static_assert(txt2.at(2) == 'c');
#if SNAP_HAS_CONSTEXPR_ITERATOR
static_assert(txt9.at(0) == 'c');
static_assert(txt9.at(1) == 'b');
static_assert(txt9.at(2) == 'a');
#endif

static_assert(txt1.front() == 'a');
static_assert(txt1.back()  == 'a');
static_assert(txt2.front() == 'a');
static_assert(txt2.back()  == 'c');
#if SNAP_HAS_STD_FROM_RANGE
static_assert(txt5.front() == 'a');
static_assert(txt5.back()  == 'c');
#endif
#if SNAP_HAS_STD_FROM_RANGE && SNAP_HAS_CONSTEXPR_STRING
static_assert(txt6.front() == 'a');
static_assert(txt6.back()  == 'c');
#endif
#if SNAP_HAS_CONSTEXPR_STRING
static_assert(txt7.front() == 'a');
static_assert(txt7.back()  == 'c');
#endif
static_assert(txt8.front() == 'a');
static_assert(txt8.back()  == 'c');
#if SNAP_HAS_CONSTEXPR_ITERATOR
static_assert(txt9.front() == 'c');
static_assert(txt9.back()  == 'a');
#endif

static_assert(std::string_view(txt0.data()) == ""); // NOLINT
static_assert(std::string_view(txt0.c_str()) == ""); // NOLINT
static_assert(std::string_view(txt1.data()) == "a");
static_assert(std::string_view(txt1.c_str()) == "a");
static_assert(std::string_view(txt2.data()) == "abc");
static_assert(std::string_view(txt2.c_str()) == "abc");

static_assert(txt0 == "");
static_assert("a" == txt1);
static_assert(txt2 == "abc");
static_assert(txt3 == "abc");
static_assert(txt4 == "abc");
#if SNAP_HAS_STD_FROM_RANGE
static_assert(txt5 == "abc");
#endif
#if SNAP_HAS_STD_FROM_RANGE && SNAP_HAS_CONSTEXPR_STRING
static_assert(txt6 == "abc");
#endif
#if SNAP_HAS_CONSTEXPR_STRING
static_assert(txt7 == "abc");
#endif
static_assert(txt8 == "abc");
#if SNAP_HAS_CONSTEXPR_ITERATOR
static_assert(txt9 == "cba");
#endif

static_assert(txt1 == snap::basic_fixed_string("a"));
static_assert(txt1 != snap::basic_fixed_string("b"));
static_assert(txt1 != snap::basic_fixed_string("aa"));
static_assert(txt1 <  snap::basic_fixed_string("b"));
static_assert(txt1 <  snap::basic_fixed_string("aa"));
static_assert(txt1 == "a");
static_assert(txt1 != "b");
static_assert(txt1 != "aa");
static_assert(txt1 <  "b");
static_assert(txt1 <  "aa");

static_assert(txt1 + snap::basic_fixed_string('b')    == "ab");
static_assert(snap::basic_fixed_string('b') + txt1    == "ba");
static_assert(txt1 + snap::basic_fixed_string("bc")   == "abc");
static_assert(snap::basic_fixed_string("bc") + txt1   == "bca");
static_assert(txt1 + 'b'                        == "ab");
static_assert('b' + txt1                        == "ba");
static_assert(txt1 + "bc"                       == "abc");
static_assert("bc" + txt1                       == "bca");

static_assert(txt2 == snap::basic_fixed_string("abc"));
static_assert(txt2 != snap::basic_fixed_string("cba"));
static_assert(txt2 != snap::basic_fixed_string("abcd"));
static_assert(txt2 <  snap::basic_fixed_string("b"));
static_assert(txt2 >  snap::basic_fixed_string("aa"));
static_assert(txt2 == "abc");
static_assert(txt2 != "cba");
static_assert(txt2 != "abcd");
static_assert(txt2 <  "b");
static_assert(txt2 >  "aa");

static_assert(txt2 + snap::basic_fixed_string('d')   == "abcd");
static_assert(snap::basic_fixed_string('d') + txt2   == "dabc");
static_assert(txt2 + snap::basic_fixed_string("def") == "abcdef");
static_assert(snap::basic_fixed_string("def") + txt2 == "defabc");
static_assert(txt2 + 'd'                       == "abcd");
static_assert('d' + txt2                       == "dabc");
static_assert(txt2 + "def"                     == "abcdef");
static_assert("def" + txt2                     == "defabc");

static_assert(std::string_view(txt2) == "abc");
static_assert(txt2.view() == "abc");
static_assert(std::string_view(txt2).find('b') == 1);
static_assert(txt2.view().find('b') == 1);

