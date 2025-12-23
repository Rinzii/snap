#include "snap/iterator/default_sentinel.hpp"

#include <gtest/gtest.h>

TEST(Iterator, DefaultSentinelIsSingleton)
{
	constexpr auto a = SNAP_NAMESPACE::default_sentinel;
	constexpr auto b = SNAP_NAMESPACE::default_sentinel;
	(void)a;
	(void)b;
	SUCCEED();
}
