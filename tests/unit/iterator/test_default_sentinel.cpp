#include <gtest/gtest.h>

#include "snap/iterator/default_sentinel.hpp"

TEST(Iterator, DefaultSentinelIsSingleton)
{
        constexpr auto a = snap::default_sentinel;
        constexpr auto b = snap::default_sentinel;
        (void)a;
        (void)b;
        SUCCEED();
}
