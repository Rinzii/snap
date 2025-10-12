#include <gtest/gtest.h>

#include "snap/memory/to_address.hpp"

#include <memory>

TEST(Memory, ToAddressReturnsRawPointer)
{
        int value = 42;
        EXPECT_EQ(snap::to_address(&value), &value);

        std::unique_ptr<int> smart(new int(99));
        EXPECT_EQ(snap::to_address(smart), smart.get());
}
