#include "snap/internal/helpers/bit_log2.hpp"

#include <gtest/gtest.h>

#include <cstdint>

TEST(Internal, BitLog2MatchesPowerOfTwo)
{
	EXPECT_EQ(SNAP_NAMESPACE::internal::bit_log2(std::uint32_t{ 1 }), 0);
	EXPECT_EQ(SNAP_NAMESPACE::internal::bit_log2(std::uint32_t{ 8 }), 3);
}
