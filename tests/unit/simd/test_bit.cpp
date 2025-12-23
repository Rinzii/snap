#include "snap/simd/simd.hpp"

#include <gtest/gtest.h>

TEST(Simd, HasSingleBitRecognisesPowersOfTwo)
{
	EXPECT_TRUE(SNAP_NAMESPACE::has_single_bit(1u));
	EXPECT_TRUE(SNAP_NAMESPACE::has_single_bit(8u));
	EXPECT_FALSE(SNAP_NAMESPACE::has_single_bit(0u));
	EXPECT_FALSE(SNAP_NAMESPACE::has_single_bit(3u));
}
