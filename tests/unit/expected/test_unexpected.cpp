#include "snap/expected/unexpected.hpp"

#include <gtest/gtest.h>

#include <string>
#include <utility>

TEST(Expected, UnexpectedStoresErrorValue)
{
	snap::unexpected<int> code(std::in_place, 404);
	EXPECT_EQ(code.error(), 404);

	snap::unexpected<std::string> message(std::in_place, "not found");
	EXPECT_EQ(message.error(), "not found");
}
