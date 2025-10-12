#include <gtest/gtest.h>

#include "snap/internal/helpers/bit_log2.hpp"

#include <cstdint>

TEST(Internal, BitLog2MatchesPowerOfTwo)
{
        EXPECT_EQ(snap::internal::bit_log2(std::uint32_t{ 1 }), 0);
        EXPECT_EQ(snap::internal::bit_log2(std::uint32_t{ 8 }), 3);
}
