#include <gtest/gtest.h>

#include "snap/testing/environment.hpp"

int main(int argc, char** argv) { // NOLINT
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::GTEST_FLAG(color) = "yes";
    ::testing::GTEST_FLAG(death_test_style) = "threadsafe";
    snap::test::InstallGlobalEnvironment();
    return RUN_ALL_TESTS();
}
