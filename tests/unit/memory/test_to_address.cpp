#include "snap/memory/to_address.hpp"

#include <gtest/gtest.h>

#include <memory>

TEST(Memory, ToAddressReturnsRawPointer)
{
	int value = 42;
	EXPECT_EQ(SNAP_NAMESPACE::to_address(&value), &value);

	const std::unique_ptr<int> smart(new int(99));
	EXPECT_EQ(SNAP_NAMESPACE::to_address(smart), smart.get());
}
