#include "snap/numeric/midpoint.hpp"

#include <gtest/gtest.h>

TEST(Numeric, MidpointHandlesIntegersAndPointers)
{
	EXPECT_EQ(SNAP_NAMESPACE::midpoint(2, 6), 4);
	EXPECT_EQ(SNAP_NAMESPACE::midpoint(6, 2), 4);

	int values[]{ 1, 2, 3, 4 };
	int* mid = SNAP_NAMESPACE::midpoint(&values[0], &values[4]);
	EXPECT_EQ(mid, &values[2]);
}
