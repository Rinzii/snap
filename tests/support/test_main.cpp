#include "snap/testing/environment.hpp"

#include <gtest/gtest.h>

int main(int argc, char** argv)
{ // NOLINT
	::testing::InitGoogleTest(&argc, argv);
	::testing::GTEST_FLAG(color)			= "yes";
	::testing::GTEST_FLAG(death_test_style) = "threadsafe";
	SNAP_NAMESPACE::test::InstallGlobalEnvironment();
	return RUN_ALL_TESTS();
}
