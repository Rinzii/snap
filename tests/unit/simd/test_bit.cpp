#include "snap/simd/simd.hpp"

#include <gtest/gtest.h>

TEST(Simd, HasSingleBitRecognisesPowersOfTwo)
{
	EXPECT_TRUE(snap::has_single_bit(1u));
	EXPECT_TRUE(snap::has_single_bit(8u));
	EXPECT_FALSE(snap::has_single_bit(0u));
	EXPECT_FALSE(snap::has_single_bit(3u));
}
