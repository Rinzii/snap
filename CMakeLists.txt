cmake_minimum_required(VERSION 3.20.0)


# ----------------------------------------------------------------------
# Project Setup and Metadata
# ----------------------------------------------------------------------
set(SNAP_TARGET_NAME "snap")
set(SNAP_BUILD_VERSION 0.2.0)

project(
        ${SNAP_TARGET_NAME}
        VERSION ${SNAP_BUILD_VERSION}
        DESCRIPTION ""
        HOMEPAGE_URL "https://github.com/Rinzii/snap"
        LANGUAGES CXX
)

# Include helper CMake files for project configuration
include(cmake/config/ProjectIsTopLevel.cmake)
include(cmake/config/ProjectOptions.cmake)
include(cmake/config/UserOptions.cmake)

snap_option(SNAP_ENABLE_ABI_NAMESPACE "Enable inline ABI namespace for snap symbols" ON ON)
set(SNAP_ABI_NAMESPACE "abi_v1" CACHE STRING "Inline ABI namespace nested inside snap")
set(SNAP_NAMESPACE "snap" CACHE STRING "Namespace of the snap library")

set(_snap_abi_namespace_pattern "^[A-Za-z_][A-Za-z0-9_]*$")
if (NOT SNAP_ABI_NAMESPACE MATCHES "${_snap_abi_namespace_pattern}")
    message(FATAL_ERROR "SNAP_ABI_NAMESPACE ('${SNAP_ABI_NAMESPACE}') must match ${_snap_abi_namespace_pattern}")
endif ()
string(SUBSTRING "${SNAP_ABI_NAMESPACE}" 0 1 _snap_abi_namespace_first_char)
if (_snap_abi_namespace_first_char STREQUAL "_")
    message(FATAL_ERROR "SNAP_ABI_NAMESPACE ('${SNAP_ABI_NAMESPACE}') must not start with '_'")
endif ()
if (SNAP_ABI_NAMESPACE MATCHES "__")
    message(FATAL_ERROR "SNAP_ABI_NAMESPACE ('${SNAP_ABI_NAMESPACE}') must not contain '__'")
endif ()

set(_snap_enable_abi_namespace_bool ${SNAP_ENABLE_ABI_NAMESPACE})
if (_snap_enable_abi_namespace_bool)
    set(SNAP_ENABLE_ABI_NAMESPACE 1)
else ()
    set(SNAP_ENABLE_ABI_NAMESPACE 0)
endif ()
unset(_snap_enable_abi_namespace_bool)


# ----------------------------------------------------------------------
# Color Diagnostics (Only if at top-level with Ninja)
# ----------------------------------------------------------------------
if (SNAP_ENABLE_PROJECT_FLAGS AND SNAP_ENABLE_COLOR_DIAGNOSTICS AND SNAP_PROJECT_IS_TOP_LEVEL)
    if (CMAKE_GENERATOR STREQUAL "Ninja")
        # Enable colored output for C++ compilers
        if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
        elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "IntelLLVM")
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
        endif ()
    endif ()
endif ()

# ----------------------------------------------------------------------
# Optional fuzzing harnesses
# ----------------------------------------------------------------------
if (SNAP_BUILD_FUZZERS)
    if (NOT SNAP_BUILD_TESTS)
        include(CTest)
        enable_testing()
    endif ()
    if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.25)
        add_subdirectory(fuzzing SYSTEM)
    else ()
        add_subdirectory(fuzzing)
    endif ()
endif ()

message(STATUS "SNAP: Version: ${SNAP_BUILD_VERSION}")

# ----------------------------------------------------------------------
# Compiler Warning Options
# ----------------------------------------------------------------------
set(SNAP_warning_options)

if (SNAP_ENABLE_PROJECT_FLAGS)
    # Collect common warning flags for various compilers and conditions.
    set(
            SNAP_warning_options
            $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wconversion -Wpedantic -Werror=return-type -Wundef -Wnull-dereference>
            $<$<CXX_COMPILER_ID:Clang>:-Wpedantic-macros>
            $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive- /we4715>
    )

    # Turn on -Werror or /WX if requested
    if (SNAP_ENABLE_WARNINGS_AS_ERRORS)
        list(
                APPEND
                SNAP_warning_options
                $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Werror>
                $<$<CXX_COMPILER_ID:MSVC>:/WX>
        )
    endif ()

    # Add extra debug info flags if requested
    if (SNAP_ENABLE_DEBUG_INFO)
        list(
                APPEND
                SNAP_warning_options
                $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wwrite-strings -g3>
                $<$<CXX_COMPILER_ID:MSVC>:/Zi /analyze>
        )
    endif ()
endif ()


# ----------------------------------------------------------------------
# Library Definition
# ----------------------------------------------------------------------
add_library(${SNAP_TARGET_NAME})
add_library(${SNAP_TARGET_NAME}::${SNAP_TARGET_NAME} ALIAS ${SNAP_TARGET_NAME})

# Collect source and header files for installation or packaging
include(cmake/helpers/SnapAddTargetSources.cmake)
add_subdirectory(include/snap)
add_subdirectory(src)

# Detect supported compiler features
include(cmake/features/GetAllSupportedFeatures.cmake)

target_include_directories(${SNAP_TARGET_NAME} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
        "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include/>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

# Specify minimum C++ standard for library (likely C++17)
target_compile_features(${SNAP_TARGET_NAME} PUBLIC ${SNAP_DESIRED_CXX_STANDARD})

# Enable aggressive warning options only if requested
if (SNAP_ENABLE_PROJECT_FLAGS AND SNAP_ENABLE_AGGRESSIVE_WARNINGS AND SNAP_warning_options)
    target_compile_options(${SNAP_TARGET_NAME} PUBLIC ${SNAP_warning_options})
endif ()

# Enable AddressSanitizer if requested
if (SNAP_ENABLE_PROJECT_FLAGS AND SNAP_ENABLE_SANITIZER_BUILD)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
endif ()

# Setup project specific compile definitions that must continue with the interface
include(cmake/config/ProjectCompileDefinitions.cmake)

# Setup the version header for the library
set(SNAP_VERSION_MAJOR ${CMAKE_PROJECT_VERSION_MAJOR})
set(SNAP_VERSION_MINOR ${CMAKE_PROJECT_VERSION_MINOR})
set(SNAP_VERSION_PATCH ${CMAKE_PROJECT_VERSION_PATCH})
configure_file("${PROJECT_SOURCE_DIR}/cmake/in/snap_version.hpp.in" "${PROJECT_BINARY_DIR}/include/snap/version.hpp")
configure_file("${PROJECT_SOURCE_DIR}/include/snap/internal/config/abi_namespace.hpp.in" "${PROJECT_BINARY_DIR}/include/snap/internal/config/abi_namespace.hpp" @ONLY)


# ----------------------------------------------------------------------
# Package config + exports
# ----------------------------------------------------------------------
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(SNAP_CMAKE_CONFIG_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(SNAP_CMAKE_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${SNAP_TARGET_NAME}")

configure_package_config_file(
        "${SNAP_ROOT_DIR}/cmake/in/snap-config.cmake.in"
        "${SNAP_CMAKE_CONFIG_BUILD_DIR}/snap-config.cmake"
        INSTALL_DESTINATION "${SNAP_CMAKE_CONFIG_INSTALL_DIR}"
)

write_basic_package_version_file(
        "${SNAP_CMAKE_CONFIG_BUILD_DIR}/snap-config-version.cmake"
        COMPATIBILITY SameMajorVersion
)

export(
        TARGETS ${SNAP_TARGET_NAME}
        FILE "${SNAP_CMAKE_CONFIG_BUILD_DIR}/${SNAP_TARGET_NAME}-targets.cmake"
        NAMESPACE ${SNAP_TARGET_NAME}::
)

export(PACKAGE ${SNAP_TARGET_NAME})


# ----------------------------------------------------------------------
# Installation Rules
# ----------------------------------------------------------------------
if (SNAP_INSTALL)
    include(cmake/config/InstallRules.cmake)
endif ()


# ----------------------------------------------------------------------
# Subdirectories for Examples, Benchmarks, and Tests
# ----------------------------------------------------------------------
# Only add these if the user explicitly asked for them in the options.
if (SNAP_BUILD_EXAMPLES OR SNAP_BUILD_BENCHMARKS OR SNAP_BUILD_TESTS)
    if (SNAP_BUILD_EXAMPLES)
        # Use SYSTEM to suppress warnings from external code if supported by cmake
        if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.25)
            add_subdirectory(examples SYSTEM)
        else ()
            add_subdirectory(examples)
        endif ()
    endif ()

    if (SNAP_BUILD_TESTS)
        include(CTest)
        enable_testing()
        # Use SYSTEM to suppress warnings from external code if supported by cmake
        if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.25)
            add_subdirectory(tests SYSTEM)
        else ()
            add_subdirectory(tests)
        endif ()
    endif ()
endif ()
