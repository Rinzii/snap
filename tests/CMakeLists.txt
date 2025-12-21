cmake_minimum_required(VERSION 3.20)

# ==================================================================
# CTest wiring
# ==================================================================
include(CTest)
enable_testing()
set(CTEST_OUTPUT_ON_FAILURE ON)

set_property(GLOBAL PROPERTY CTEST_LABELS_FOR_SUBPROJECTS
        "unit;compile-fail;bit;functional;type_traits;integration"
)

# ==================================================================
# Detect supported C++ dialects
# ==================================================================
include(CheckCXXCompilerFlag)

if (MSVC)
  set(SNAP_FLAG_CXX17 "/std:c++17")
  set(SNAP_FLAG_CXX20 "/std:c++20")
  set(SNAP_FLAG_CXX23 "/std:c++latest")
else()
  set(SNAP_FLAG_CXX17 "-std=c++17")
  set(SNAP_FLAG_CXX20 "-std=c++20")
  set(SNAP_FLAG_CXX23 "-std=c++23")
endif()

check_cxx_compiler_flag("${SNAP_FLAG_CXX17}" SNAP_HAS_CXX17)
check_cxx_compiler_flag("${SNAP_FLAG_CXX20}" SNAP_HAS_CXX20)
check_cxx_compiler_flag("${SNAP_FLAG_CXX23}" SNAP_HAS_CXX23)

function(_snap_std_supported out std)
  if (std EQUAL 17)
    set(${out} ${SNAP_HAS_CXX17} PARENT_SCOPE)
  elseif (std EQUAL 20)
    set(${out} ${SNAP_HAS_CXX20} PARENT_SCOPE)
  elseif (std EQUAL 23)
    set(${out} ${SNAP_HAS_CXX23} PARENT_SCOPE)
  else()
    set(${out} FALSE PARENT_SCOPE)
  endif()
endfunction()

# ==================================================================
# GoogleTest (prefer system, fallback to FetchContent)
# ==================================================================
set(SNAP_GTEST_VERSION 1.17.0)
set(SNAP_GTEST_VERSION_VAR "v${SNAP_GTEST_VERSION}")

find_package(GTest ${SNAP_GTEST_VERSION} QUIET CONFIG)
if (NOT GTest_FOUND)
  include(FetchContent)
  FetchContent_Declare(
          googletest
          GIT_REPOSITORY https://github.com/google/googletest
          GIT_TAG ${SNAP_GTEST_VERSION_VAR}
  )
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  if (TARGET gtest AND NOT TARGET GTest::gtest)
    add_library(GTest::gtest ALIAS gtest)
  endif()
  if (TARGET gmock AND NOT TARGET GTest::gmock)
    add_library(GTest::gmock ALIAS gmock)
  endif()
endif()

# ==================================================================
# Shared test main / helpers
# ==================================================================
add_library(snap_test_support STATIC support/test_main.cpp)
target_link_libraries(snap_test_support PUBLIC GTest::gtest GTest::gmock)
target_compile_features(snap_test_support PUBLIC cxx_std_17)
target_include_directories(snap_test_support PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/support
)
set_property(TARGET snap_test_support PROPERTY FOLDER "tests/_support")

# ==================================================================
# Hardening for test targets
# ==================================================================
function(_snap_harden target)
  if (MSVC)
    if (SNAP_TEST_WERROR)
      target_compile_options(${target} PRIVATE /WX)
    endif()
    if (SNAP_TEST_NO_RTTI)
      target_compile_options(${target} PRIVATE /GR-)
    endif()
    if (SNAP_TEST_NO_EXCEPTIONS)
      target_compile_options(${target} PRIVATE /EHs-)
    endif()
    if (SNAP_TEST_ENABLE_ASAN)
      target_compile_options(${target} PRIVATE /fsanitize=address)
      target_link_options(${target} PRIVATE /fsanitize=address)
    endif()
  else()
    if (SNAP_TEST_WERROR)
      target_compile_options(${target} PRIVATE -Werror)
    endif()
    if (SNAP_TEST_NO_RTTI)
      target_compile_options(${target} PRIVATE -fno-rtti)
    endif()
    if (SNAP_TEST_NO_EXCEPTIONS)
      target_compile_options(${target} PRIVATE -fno-exceptions)
    endif()
    if (SNAP_TEST_ENABLE_TSAN)
      target_compile_options(${target} PRIVATE -fsanitize=thread)
      target_link_options(${target} PRIVATE -fsanitize=thread)
    elseif (SNAP_TEST_ENABLE_ASAN)
      target_compile_options(${target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
      target_link_options(${target} PRIVATE -fsanitize=address,undefined)
    endif()
  endif()
endfunction()

# ==================================================================
# Coverage (gcc/clang)
# ==================================================================
if (SNAP_TEST_COVERAGE AND NOT MSVC)
  add_compile_options(-O0 -g --coverage)
  add_link_options(--coverage)
  add_custom_target(coverage
          COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -j
          COMMAND bash -c "lcov -q -d . -c -o coverage.info && lcov -q -r coverage.info '*/thirdparty/*' '*/tests/*' -o coverage.info && genhtml -q coverage.info -o coverage_html"
          WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
  set_property(TARGET coverage PROPERTY FOLDER "tests/_utils")
endif()

# ==================================================================
# Helper: add gtest target(s) per C++ standard
# ==================================================================
function(snap_add_gtest name)
  set(oneValueArgs FOLDER)
  set(multiValueArgs SOURCES LIBS LABELS STANDARDS INCLUDES HEADERS DEFINES)
  cmake_parse_arguments(ARG "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if (NOT ARG_STANDARDS)
    set(ARG_STANDARDS 17)
  endif()

  foreach(std IN LISTS ARG_STANDARDS)
    _snap_std_supported(SUP ${std})
    if (NOT SUP)
      message(STATUS "Skipping ${name} for C++${std} (unsupported).")
      continue()
    endif()

    set(tgt ${name}_cxx${std})
    add_executable(${tgt} ${ARG_SOURCES})
    if (ARG_HEADERS)
      target_sources(${tgt} PRIVATE ${ARG_HEADERS})
      foreach(_h IN LISTS ARG_HEADERS)
        if (EXISTS "${_h}")
          set_source_files_properties(${_h} PROPERTIES HEADER_FILE_ONLY TRUE)
        endif()
      endforeach()
    endif()

    target_link_libraries(${tgt} PRIVATE snap snap_test_support ${ARG_LIBS})
    target_compile_features(${tgt} PRIVATE cxx_std_${std})
    target_compile_definitions(${tgt} PRIVATE ${ARG_DEFINES}
            SNAP_TEST_DATA_DIR="${CMAKE_CURRENT_LIST_DIR}/data")
    target_include_directories(${tgt} PRIVATE
            ${CMAKE_SOURCE_DIR}/include        # snap headers (default)
            ${CMAKE_BINARY_DIR}/include        # generated headers (version/abi)
            ${CMAKE_CURRENT_LIST_DIR}
            ${CMAKE_CURRENT_LIST_DIR}/support
            ${ARG_INCLUDES}
    )

    set_property(TARGET ${tgt} PROPERTY CXX_STANDARD ${std})
    set_property(TARGET ${tgt} PROPERTY CXX_STANDARD_REQUIRED ON)
    if (ARG_FOLDER)
      set_property(TARGET ${tgt} PROPERTY FOLDER "${ARG_FOLDER}/cxx${std}")
    else()
      set_property(TARGET ${tgt} PROPERTY FOLDER "tests/cxx${std}")
    endif()
    _snap_harden(${tgt})

    # Labels
    set(_labels "${ARG_LABELS};cxx${std}")

    if (SNAP_TEST_GROUPED)
      add_test(NAME ${tgt}_all
              COMMAND $<TARGET_FILE:${tgt}> --gtest_color=yes)
      set_tests_properties(${tgt}_all PROPERTIES
              LABELS "${_labels}"
              WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    else()
      include(GoogleTest)
      string(REPLACE ";" "\\;" _labels_escaped "${_labels}")
      gtest_discover_tests(${tgt}
              WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
              TEST_PREFIX "${tgt}."
              DISCOVERY_TIMEOUT 30
              PROPERTIES LABELS "${_labels_escaped}"
      )
    endif()
  endforeach()
endfunction()

# ==================================================================
# Helper: register unit tests by *sources only* with optional per-test overrides
#
# Usage A (simple, recommended):
#   snap_add_unit_tests(
#     NAME bit
#     SOURCES
#       bit/test_bit_cast.cpp
#       bit/test_bit_ceil.cpp
#     STANDARDS 17;20
#     LIBS <global-extra-libs...>
#     INCLUDES <global-extra-include-dirs...>
#     HEADERS <extra-header-files-for-IDE...>
#   )
#
# Usage B (per-test overrides):
#   CASES entries allow: "src[|lib1,lib2][|inc1,inc2][|hdr1,hdr2]"
# ==================================================================
function(snap_add_unit_tests)
  set(oneValueArgs NAME)
  set(multiValueArgs SOURCES CASES STANDARDS LIBS INCLUDES HEADERS DEFINES)
  cmake_parse_arguments(ARG "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if (NOT ARG_NAME)
    message(FATAL_ERROR "snap_add_unit_tests: NAME is required")
  endif()
  if (NOT ARG_SOURCES AND NOT ARG_CASES)
    message(FATAL_ERROR "snap_add_unit_tests: provide SOURCES or CASES")
  endif()
  if (NOT ARG_STANDARDS)
    set(ARG_STANDARDS 17)
  endif()

  # Helper: resolve a source under tests/unit if relative
  function(_snap_resolve_test_src out path)
    if (IS_ABSOLUTE "${path}")
      set(${out} "${path}" PARENT_SCOPE)
    else()
      # Default to tests/unit/<path>
      set(_cand "${CMAKE_CURRENT_LIST_DIR}/unit/${path}")
      if (EXISTS "${_cand}")
        set(${out} "${_cand}" PARENT_SCOPE)
      else()
        # Fallback: treat as relative to tests/ (rare)
        set(_cand2 "${CMAKE_CURRENT_LIST_DIR}/${path}")
        set(${out} "${_cand2}" PARENT_SCOPE)
      endif()
    endif()
  endfunction()

  # Helper: resolve an include dir header or extra header path if relative
  function(_snap_resolve_path out path)
    if (IS_ABSOLUTE "${path}")
      set(${out} "${path}" PARENT_SCOPE)
    else()
      set(_a "${CMAKE_CURRENT_LIST_DIR}/${path}")
      if (EXISTS "${_a}")
        set(${out} "${_a}" PARENT_SCOPE)
      else()
        set(_b "${CMAKE_SOURCE_DIR}/${path}")
        set(${out} "${_b}" PARENT_SCOPE)
      endif()
    endif()
  endfunction()

  # Build targets from simple SOURCES (same libs/includes for all)
  foreach(src IN LISTS ARG_SOURCES)
    _snap_resolve_test_src(_src "${src}")
    if (NOT EXISTS "${_src}")
      message(FATAL_ERROR "snap_add_unit_tests: source not found: ${src} (resolved '${_src}')")
    endif()

    get_filename_component(_base "${_src}" NAME_WE)
    snap_add_gtest(snap_${ARG_NAME}_${_base}
            SOURCES "${_src}"
            STANDARDS ${ARG_STANDARDS}
            LIBS ${ARG_LIBS}
            INCLUDES ${ARG_INCLUDES}
            HEADERS ${ARG_HEADERS}
            DEFINES ${ARG_DEFINES}
            LABELS unit;${ARG_NAME};${ARG_NAME}:${_base}
            FOLDER "tests/${ARG_NAME}"
    )
  endforeach()

  # Build targets from CASES with per-test overrides
  foreach(case IN LISTS ARG_CASES)
    string(REPLACE "|" ";" _parts "${case}")
    list(LENGTH _parts _len)
    if (_len LESS 1)
      message(FATAL_ERROR "snap_add_unit_tests: bad CASE entry '${case}'")
    endif()

    list(GET _parts 0 _src_in)
    _snap_resolve_test_src(_src "${_src_in}")
    if (NOT EXISTS "${_src}")
      message(FATAL_ERROR "snap_add_unit_tests: source not found in CASE '${case}' (resolved '${_src}')")
    endif()

    set(_case_libs "")
    set(_case_includes "")
    set(_case_headers "")

    if (_len GREATER 1)
      list(GET _parts 1 _libs_raw)
      if (NOT _libs_raw STREQUAL "")
        string(REPLACE "," ";" _case_libs "${_libs_raw}")
      endif()
    endif()
    if (_len GREATER 2)
      list(GET _parts 2 _incs_raw)
      if (NOT _incs_raw STREQUAL "")
        string(REPLACE "," ";" _incs_list "${_incs_raw}")
        foreach(i IN LISTS _incs_list)
          _snap_resolve_path(_ri "${i}")
          list(APPEND _case_includes "${_ri}")
        endforeach()
      endif()
    endif()
    if (_len GREATER 3)
      list(GET _parts 3 _hdrs_raw)
      if (NOT _hdrs_raw STREQUAL "")
        string(REPLACE "," ";" _hdrs_list "${_hdrs_raw}")
        foreach(h IN LISTS _hdrs_list)
          _snap_resolve_path(_rh "${h}")
          list(APPEND _case_headers "${_rh}")
        endforeach()
      endif()
    endif()

    get_filename_component(_base "${_src}" NAME_WE)
    snap_add_gtest(snap_${ARG_NAME}_${_base}
            SOURCES "${_src}"
            STANDARDS ${ARG_STANDARDS}
            LIBS ${ARG_LIBS} ${_case_libs}
            INCLUDES ${ARG_INCLUDES} ${_case_includes}
            HEADERS ${ARG_HEADERS} ${_case_headers}
            DEFINES ${ARG_DEFINES}
            LABELS unit;${ARG_NAME};${ARG_NAME}:${_base}
            FOLDER "tests/${ARG_NAME}"
    )
  endforeach()
endfunction()

# ==================================================================
# Helper: compile-fail test (expects compiler error)
# ==================================================================
function(snap_add_compile_fail)
  set(oneValueArgs NAME FILE STD FOLDER)
  set(multiValueArgs INCLUDES DEFINES FLAGS LABELS EXPECT)
  cmake_parse_arguments(ARG "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if (NOT ARG_NAME)
    message(FATAL_ERROR "snap_add_compile_fail: NAME is required")
  endif()
  if (NOT ARG_FILE)
    message(FATAL_ERROR "snap_add_compile_fail: FILE is required")
  endif()
  if (NOT EXISTS "${ARG_FILE}")
    message(FATAL_ERROR "snap_add_compile_fail: FILE '${ARG_FILE}' not found")
  endif()
  if (NOT ARG_STD)
    set(ARG_STD 17)
  endif()

  # default include dirs for tests + extras
  set(_includes_list
          "${CMAKE_SOURCE_DIR}/include"
          "${CMAKE_CURRENT_LIST_DIR}"
          "${CMAKE_CURRENT_LIST_DIR}/support"
  )
  list(APPEND _includes_list ${ARG_INCLUDES})

  # make sure STL headers resolve under raw compiler invocation
  if (MSVC)
    list(APPEND _includes_list ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
  endif()

  set(_incs "")
  foreach(dir IN LISTS _includes_list)
    if (MSVC)
      list(APPEND _incs /I "${dir}")
    else()
      list(APPEND _incs -I "${dir}")
    endif()
  endforeach()

  set(_defs "")
  foreach(def IN LISTS ARG_DEFINES)
    if (MSVC)
      list(APPEND _defs /D "${def}")
    else()
      list(APPEND _defs -D"${def}")
    endif()
  endforeach()

  if (MSVC)
    if (ARG_STD EQUAL 17)
      set(_std ${SNAP_FLAG_CXX17})
    elseif (ARG_STD EQUAL 20)
      set(_std ${SNAP_FLAG_CXX20})
    elseif (ARG_STD EQUAL 23)
      set(_std ${SNAP_FLAG_CXX23})
    else()
      message(FATAL_ERROR "Unsupported STD: ${ARG_STD}")
    endif()
    set(_syntax /Zs)
    set(_no_werror /WX-)
    set(_cmd ${CMAKE_CXX_COMPILER} /nologo ${_std} ${_syntax} ${_no_werror} ${_defs} ${_incs} ${ARG_FLAGS} "${ARG_FILE}")
  else()
    if (ARG_STD EQUAL 17)
      set(_std ${SNAP_FLAG_CXX17})
    elseif (ARG_STD EQUAL 20)
      set(_std ${SNAP_FLAG_CXX20})
    elseif (ARG_STD EQUAL 23)
      set(_std ${SNAP_FLAG_CXX23})
    else()
      message(FATAL_ERROR "Unsupported STD: ${ARG_STD}")
    endif()
    set(_syntax -fsyntax-only)
    set(_no_werror -Wno-error)
    set(_cmd ${CMAKE_CXX_COMPILER} ${_std} ${_syntax} ${_no_werror} ${_defs} ${_incs} ${ARG_FLAGS} "${ARG_FILE}")
  endif()

  set(_testname ${ARG_NAME}_cxx${ARG_STD})
  add_test(NAME ${_testname} COMMAND ${_cmd})

  set(_labels "${ARG_LABELS};compile-fail;cxx${ARG_STD}")
  set_tests_properties(${_testname} PROPERTIES
          WILL_FAIL TRUE
          LABELS "${_labels}"
          USES_TERMINAL TRUE
  )
  if (ARG_EXPECT)
    set_tests_properties(${_testname} PROPERTIES PASS_REGULAR_EXPRESSION "${ARG_EXPECT}")
  endif()

  if (ARG_FOLDER)
    add_custom_target(${_testname}_placeholder)
    set_property(TARGET ${_testname}_placeholder PROPERTY FOLDER "${ARG_FOLDER}/compile-fail")
  endif()
endfunction()

# ==================================================================
# Integration tests
# ==================================================================
add_subdirectory(integration)

# ==================================================================
# bit (unit tests)
# Sources are relative to tests/unit/.
# ==================================================================
snap_add_unit_tests(
        NAME bit
        STANDARDS 17;20
        SOURCES
        bit/test_bit_cast.cpp
        bit/test_bit_ceil.cpp
        bit/test_bit_floor.cpp
        bit/test_bit_width.cpp
        bit/test_byteswap.cpp
        bit/test_countl_countr.cpp
        bit/test_popcount.cpp
        bit/test_rotl_rotr.cpp
        bit/test_has_single_bit.cpp
        bit/test_endian.cpp
)

# ==================================================================
# fixed_string (unit tests)
# test file lives directly under tests/unit/
# ==================================================================
option(SNAP_ENABLE_FIXED_STRING_CXX20_TESTS "Enable C++20 builds of fixed_string tests" OFF)
set(_snap_fixed_string_standards 17)
if (SNAP_ENABLE_FIXED_STRING_CXX20_TESTS)
  list(APPEND _snap_fixed_string_standards 20)
endif()

snap_add_unit_tests(
        NAME fixed_string
        STANDARDS ${_snap_fixed_string_standards}
        SOURCES
        test_fixed_string.cpp
        fixed_string/test_runtime.cpp
)

# ==================================================================
# numbers (unit tests)
# ==================================================================
snap_add_unit_tests(
        NAME numbers
        STANDARDS 17;20
        SOURCES
        numbers/test_constants.cpp
)

# ==================================================================
# Additional baseline coverage per module
# ==================================================================
snap_add_unit_tests(
        NAME concepts
        STANDARDS 20
        SOURCES
        concepts/test_ranges.cpp
)

snap_add_unit_tests(
        NAME debugging
        STANDARDS 17
        SOURCES
        debugging/test_debugger.cpp
)

snap_add_unit_tests(
        NAME expected
        STANDARDS 17
        SOURCES
        expected/test_unexpected.cpp
)

snap_add_unit_tests(
        NAME functional
        STANDARDS 17
        SOURCES
        functional/test_identity.cpp
)

snap_add_unit_tests(
        NAME internal
        STANDARDS 17
        SOURCES
        internal/test_bit_log2.cpp
)

snap_add_unit_tests(
        NAME iterator
        STANDARDS 17
        SOURCES
        iterator/test_default_sentinel.cpp
)

snap_add_unit_tests(
        NAME memory
        STANDARDS 17
        SOURCES
        memory/test_to_address.cpp
)

snap_add_unit_tests(
        NAME meta
        STANDARDS 17
        SOURCES
        meta/test_detector.cpp
)

snap_add_unit_tests(
        NAME numeric
        STANDARDS 17
        SOURCES
        numeric/test_midpoint.cpp
)

snap_add_unit_tests(
        NAME simd
        STANDARDS 17
        SOURCES
        simd/test_bit.cpp
)

snap_add_unit_tests(
        NAME stop_token
        STANDARDS 17
        SOURCES
        stop_token/test_atomic_unique_lock.cpp
)

snap_add_unit_tests(
        NAME thread
        STANDARDS 17
        SOURCES
        thread/test_jthread.cpp
)

snap_add_unit_tests(
        NAME type_traits
        STANDARDS 17
        SOURCES
        type_traits/test_remove_cvref.cpp
)

snap_add_unit_tests(
        NAME utility
        STANDARDS 17
        SOURCES
        utility/test_in_range.cpp
        utility/test_to_underlying.cpp
)

# ==================================================================
# smoke coverage (unit tests)
# Provides a thin slice of coverage across the major headers so that
# new functionality has a place to grow additional assertions.
# ==================================================================
snap_add_unit_tests(
        NAME smoke
        STANDARDS 17;20
        SOURCES
        smoke/test_core.cpp
)

option(SNAP_ENABLE_INPLACE_VECTOR_TESTS "Enable experimental inplace_vector tests" OFF)
if (SNAP_ENABLE_INPLACE_VECTOR_TESTS)
  snap_add_unit_tests(
          NAME inplace_vector
          STANDARDS 17
          SOURCES
          inplace_vector/test_basic.cpp
  )
endif()

# ==================================================================
# Compile-fail examples for bit
# ==================================================================
snap_add_compile_fail(
        NAME bad_bit_cast_non_trivial
        FILE ${CMAKE_CURRENT_LIST_DIR}/compile_fail/bit/bad_bit_cast_non_trivial.cpp
        STD 17
        LABELS bit;bit:bit_cast
        FOLDER "tests/bit"
)

snap_add_compile_fail(
        NAME bad_bit_cast_size_mismatch
        FILE ${CMAKE_CURRENT_LIST_DIR}/compile_fail/bit/bad_bit_cast_size_mismatch.cpp
        STD 17
        LABELS bit;bit:bit_cast
        FOLDER "tests/bit"
)

snap_add_compile_fail(
        NAME bad_bit_ceil_floating
        FILE ${CMAKE_CURRENT_LIST_DIR}/compile_fail/bit/bad_bit_ceil_floating.cpp
        STD 17
        LABELS bit;bit:bit_ceil
        FOLDER "tests/bit"
)

snap_add_compile_fail(
        NAME bad_bit_ceil_overflow_constexpr
        FILE ${CMAKE_CURRENT_LIST_DIR}/compile_fail/bit/bad_bit_ceil_overflow_constexpr.cpp
        STD 17
        LABELS bit;bit:bit_ceil
        FOLDER "tests/bit"
)

snap_add_compile_fail(
        NAME bad_bit_ceil_signed
        FILE ${CMAKE_CURRENT_LIST_DIR}/compile_fail/bit/bad_bit_ceil_signed.cpp
        STD 17
        LABELS bit;bit:bit_ceil
        FOLDER "tests/bit"
)

# ==================================================================
# Optional: install/consumer check (OFF by default)
# ==================================================================
if (SNAP_TEST_INSTALL)
  include(ExternalProject)
  ExternalProject_Add(install_check
          SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/support/install_check_proj
          CMAKE_ARGS
          -DCMAKE_PREFIX_PATH=${CMAKE_BINARY_DIR}/install
          -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
          INSTALL_COMMAND ""
          BUILD_ALWAYS TRUE
  )
  add_dependencies(install_check snap)
  set_property(TARGET install_check PROPERTY FOLDER "tests/_utils")
endif()

# ==================================================================
# Convenience run targets / grouping
# ==================================================================
add_custom_target(check
        COMMAND ${CMAKE_CTEST_COMMAND} -L unit --output-on-failure --label-summary -j
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
set_property(TARGET check PROPERTY FOLDER "tests/_utils")

add_custom_target(test_bit
        COMMAND ${CMAKE_CTEST_COMMAND} -L bit --output-on-failure --label-summary -j
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
set_property(TARGET test_bit PROPERTY FOLDER "tests/_utils")

add_custom_target(test_compile_fail
        COMMAND ${CMAKE_CTEST_COMMAND} -L compile-fail --output-on-failure --label-summary -j
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
set_property(TARGET test_compile_fail PROPERTY FOLDER "tests/_utils")

add_custom_target(test_integration
        COMMAND ${CMAKE_CTEST_COMMAND} -L integration --output-on-failure --label-summary -j
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
set_property(TARGET test_integration PROPERTY FOLDER "tests/_utils")

add_custom_target(test_cxx17
        COMMAND ${CMAKE_CTEST_COMMAND} -R "_cxx17\\." --output-on-failure -j
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
set_property(TARGET test_cxx17 PROPERTY FOLDER "tests/_utils")

add_custom_target(test_cxx20
        COMMAND ${CMAKE_CTEST_COMMAND} -R "_cxx20\\." --output-on-failure -j
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
set_property(TARGET test_cxx20 PROPERTY FOLDER "tests/_utils")

if (SNAP_TEST_JUNIT)
  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/test-results")
  add_custom_target(test_junit
          COMMAND ${CMAKE_CTEST_COMMAND} --output-junit "${CMAKE_BINARY_DIR}/test-results/ctest-junit.xml" --output-on-failure -j
          WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
  set_property(TARGET test_junit PROPERTY FOLDER "tests/_utils")
endif()
