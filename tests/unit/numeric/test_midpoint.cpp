#include "snap/numeric/midpoint.hpp"

#include <gtest/gtest.h>

TEST(Numeric, MidpointHandlesIntegersAndPointers)
{
	EXPECT_EQ(snap::midpoint(2, 6), 4);
	EXPECT_EQ(snap::midpoint(6, 2), 4);

	int values[]{ 1, 2, 3, 4 };
	int* mid = snap::midpoint(&values[0], &values[4]);
	EXPECT_EQ(mid, &values[2]);
}
