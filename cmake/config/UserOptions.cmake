# ==================================================================
# Core build toggles
# ==================================================================
snap_option(SNAP_BUILD_TESTS "Build snap's test suite" ON OFF)
snap_option(SNAP_BUILD_EXAMPLES "Build the example targets" OFF OFF)
snap_option(SNAP_BUILD_BENCHMARKS "Build the benchmark targets" OFF OFF)
snap_option(SNAP_BUILD_FUZZERS "Build libFuzzer harnesses for snap" OFF OFF)
snap_option(SNAP_INSTALL "Generate installable targets + CMake package files" ON OFF)

# ==================================================================
# Project-wide flag controls
# ==================================================================
snap_option(SNAP_ENABLE_PROJECT_FLAGS "Allow snap to add its default compiler/linker flags" ON OFF)
snap_option(SNAP_ENABLE_COLOR_DIAGNOSTICS "Enable colored compiler diagnostics when supported" ON OFF)
snap_option(SNAP_ENABLE_AGGRESSIVE_WARNINGS "Enable snap's default warning flag set" ON OFF)
snap_option(SNAP_ENABLE_WARNINGS_AS_ERRORS "Treat warnings as errors in snap targets" OFF OFF)
snap_option(SNAP_ENABLE_DEBUG_INFO "Inject extra debug information flags when supported" OFF OFF)
snap_option(SNAP_ENABLE_SANITIZER_BUILD "Enable AddressSanitizer when building snap" OFF OFF)
snap_option(SNAP_ENABLE_RUNTIME_SIMD "Enable runtime SIMD feature detection" ON ON)
snap_option(SNAP_DISABLE_SVML_USAGE "Skip Intel SVML detection during SIMD checks" OFF OFF)
snap_option(SNAP_DISABLE_CMAKE_FEATURE_CHECKS "Skip snap's feature detection CMake checks" OFF OFF)
snap_option(SNAP_DISABLE_FETCHCONTENT "Disallow FetchContent fallbacks for dependencies" OFF OFF)

# ==================================================================
# Test-specific toggles
# ==================================================================
snap_option(SNAP_TEST_ENABLE_ASAN "Enable Address/UBSan for tests" OFF OFF)
snap_option(SNAP_TEST_ENABLE_TSAN "Enable ThreadSanitizer for tests" OFF OFF)
snap_option(SNAP_TEST_WERROR "Treat warnings as errors in tests" OFF OFF)
snap_option(SNAP_TEST_NO_EXCEPTIONS "Build tests with no exceptions" OFF OFF)
snap_option(SNAP_TEST_NO_RTTI "Build tests with no RTTI" OFF OFF)
snap_option(SNAP_TEST_COVERAGE "Enable coverage (gcc/clang)" OFF OFF)
snap_option(SNAP_TEST_INSTALL "Build tiny consumer using installed package" OFF OFF)
snap_option(SNAP_TEST_JUNIT "Emit JUnit XML with ctest" OFF OFF)
snap_option(
        SNAP_TEST_GROUPED
        "If ON: register one test per gtest binary (nice IDE grouping). If OFF: discover individual test cases (better CI granularity)."
        ON
        ON
)
