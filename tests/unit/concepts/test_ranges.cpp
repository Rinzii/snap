#include "snap/concepts/container_compatible_range.hpp"

#include <gtest/gtest.h>

#include <string>
#include <vector>

TEST(Concepts, ContainerCompatibleRangeRecognisesVectors)
{
#if SNAP_HAS_CPP20
	static_assert(SNAP_NAMESPACE::concepts::container_compatible_range<std::vector<int>, int>);
	static_assert(!SNAP_NAMESPACE::concepts::container_compatible_range<std::vector<int>, std::string>);
	SUCCEED();
#else
	GTEST_SKIP() << "container_compatible_range requires C++20 ranges support";
#endif
}
