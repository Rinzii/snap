#include "snap/expected/unexpected.hpp"

#include <gtest/gtest.h>

#include <string>
#include <utility>

TEST(Expected, UnexpectedStoresErrorValue)
{
	SNAP_NAMESPACE::unexpected<int> code(std::in_place, 404);
	EXPECT_EQ(code.error(), 404);

	SNAP_NAMESPACE::unexpected<std::string> message(std::in_place, "not found");
	EXPECT_EQ(message.error(), "not found");
}
