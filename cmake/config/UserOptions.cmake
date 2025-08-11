
# ==================================================================
# Options (opt-in)
# ==================================================================
option(SNAP_TEST_ENABLE_ASAN       "Enable Address/UBSan for tests" OFF)
option(SNAP_TEST_ENABLE_TSAN       "Enable ThreadSanitizer for tests" OFF)
option(SNAP_TEST_WERROR            "Treat warnings as errors in tests" OFF)
option(SNAP_TEST_NO_EXCEPTIONS     "Build tests with no exceptions" OFF)
option(SNAP_TEST_NO_RTTI           "Build tests with no RTTI" OFF)
option(SNAP_TEST_COVERAGE          "Enable coverage (gcc/clang)" OFF)
option(SNAP_TEST_INSTALL           "Build tiny consumer using installed package" OFF)
option(SNAP_TEST_JUNIT             "Emit JUnit XML with ctest" OFF)
option(SNAP_TEST_GROUPED           "If ON: register one test per gtest binary (nice IDE grouping). If OFF: discover individual test cases (better CI granularity)." ON)

