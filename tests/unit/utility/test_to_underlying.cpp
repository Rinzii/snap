#include <gtest/gtest.h>

#include "snap/utility/to_underlying.hpp"

enum class level : unsigned char
{
        low  = 1,
        high = 2,
};

TEST(Utility, ToUnderlyingCastsEnums)
{
        EXPECT_EQ(snap::to_underlying(level::low), static_cast<unsigned char>(1));
        EXPECT_EQ(snap::to_underlying(level::high), static_cast<unsigned char>(2));
}
