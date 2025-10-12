#include <gtest/gtest.h>

#include "snap/functional/identity.hpp"

#include <string>
#include <type_traits>
#include <utility>

TEST(Functional, IdentityReturnsForwardedValue)
{
        snap::identity id;
        int value = 7;
        EXPECT_EQ(id(value), 7);
        EXPECT_TRUE((std::is_same_v<decltype(id(std::move(value))), int&&>));

        std::string text = "snap";
        EXPECT_EQ(id(text), "snap");
}
