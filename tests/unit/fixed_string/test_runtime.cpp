#include "snap/internal/abi_namespace.hpp"

#include <algorithm>

SNAP_BEGIN_NAMESPACE
using std::swap_ranges;
SNAP_END_NAMESPACE

#include "snap/fixed_string.hpp"

#include "snap/testing/assertions.hpp"
#include <array>
#include <string>
#include <string_view>
#include <type_traits>

SNAP_BEGIN_NAMESPACE
namespace test_cases
{

TEST(FixedStringRuntime, BasicAccessors)
{
        constexpr auto fs = snap::fixed_string<4>{"snap"};

        EXPECT_EQ(4u, fs.size());
        EXPECT_EQ('s', fs.front());
        EXPECT_EQ('p', fs.back());
        EXPECT_EQ('n', fs[1]);
        EXPECT_EQ('a', fs.at(2));
        EXPECT_FALSE(fs.empty());
        EXPECT_EQ(std::string_view("snap"), fs.view());
        EXPECT_EQ(std::string_view("snap"), std::string_view(fs));
}

TEST(FixedStringRuntime, IteratorsBehaveLikePointers)
{
        constexpr auto fs = snap::fixed_string<3>{"abc"};

        static_assert(std::is_same_v<decltype(fs.begin()), const char*>);
        static_assert(std::is_same_v<decltype(fs.end()), const char*>);

        snap::test::ExpectRangeEq(fs, std::array{ 'a', 'b', 'c' });

        std::array<char, 3> reversed{};
        std::copy(fs.rbegin(), fs.rend(), reversed.begin());
        snap::test::ExpectRangeEq(reversed, std::array{ 'c', 'b', 'a' });
}

TEST(FixedStringRuntime, ConcatenationProducesNewFixedString)
{
        constexpr auto hello  = snap::fixed_string<5>{"hello"};
        constexpr auto space  = snap::basic_fixed_string(' ');
        constexpr auto world  = snap::fixed_string<5>{"world"};
        constexpr auto phrase = hello + space + world;

        snap::test::ExpectRangeEq(phrase,
                std::array{ 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd' });
}

TEST(FixedStringRuntime, FromIterators)
{
        const std::string text = "literals";
        snap::basic_fixed_string<char, 8> fs(text.begin(), text.end());

        snap::test::ExpectRangeEq(fs, std::array{ 'l', 'i', 't', 'e', 'r', 'a', 'l', 's' });
}

TEST(FixedStringRuntime, ThrowsWhenIndexOutOfRange)
{
        const auto fs = snap::fixed_string<3>{"abc"};

        SNAP_EXPECT_THROW_MSG((void)fs.at(5), std::out_of_range, "basic_fixed_string::at");
}

} // namespace test_cases
SNAP_END_NAMESPACE
