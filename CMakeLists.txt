cmake_minimum_required(VERSION 3.18.0)


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


# ----------------------------------------------------------------------
# Color Diagnostics (Only if at top-level with Ninja)
# ----------------------------------------------------------------------
if (SNAP_PROJECT_IS_TOP_LEVEL)
    if (CMAKE_GENERATOR STREQUAL "Ninja")
        # Enable colored output for C++ compilers
        if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
        elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "IntelLLVM")
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
        endif ()
    endif ()
endif ()

message(STATUS "SNAP: Version: ${SNAP_BUILD_VERSION}")

# Load user-configurable options (e.g., BUILD_EXAMPLES, BUILD_TESTS, etc.)
include(cmake/config/UserOptions.cmake)


# ----------------------------------------------------------------------
# Compiler Warning Options
# ----------------------------------------------------------------------
# Collect common warning flags for various compilers and conditions.
set(
        SNAP_warning_options
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wconversion -Wpedantic -Werror=return-type -Wundef -Wnull-dereference>
        $<$<CXX_COMPILER_ID:Clang>:-Wpedantic-macros>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive- /we4715>
)

# Turn on -Werror or /WX if requested
if (SNAP_ENABLE_WARNINGS_AS_ERRORS)
    set(
            SNAP_warning_options
            ${SNAP_warning_options}
            $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Werror>
            $<$<CXX_COMPILER_ID:MSVC>:/WX>
    )
endif ()

# Add extra debug info flags if requested
if (SNAP_ENABLE_DEBUG_INFO)
    set(
            SNAP_warning_options
            ${SNAP_warning_options}
            $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wwrite-strings -g3>
            $<$<CXX_COMPILER_ID:MSVC>:/Zi /analyze>
    )
endif ()


# ----------------------------------------------------------------------
# Library Definition
# ----------------------------------------------------------------------
add_library(${SNAP_TARGET_NAME} INTERFACE)
add_library(${SNAP_TARGET_NAME}::${SNAP_TARGET_NAME} ALIAS ${SNAP_TARGET_NAME})

# Collect all header files for installation or packaging
include(cmake/helpers/SnapAddHeaders.cmake) # Helper for adding headers to interface
add_subdirectory(include/SNAP)

# Detect supported compiler features
include(cmake/features/GetAllSupportedFeatures.cmake)

target_include_directories(${SNAP_TARGET_NAME} INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

# Specify minimum C++ standard for library (likely C++17)
target_compile_features(
        ${SNAP_TARGET_NAME}
        INTERFACE
        ${SNAP_DESIRED_CXX_STANDARD}
)

# Enable aggressive warning options only if requested
if (SNAP_ENABLE_AGGRESSIVE_WARNINGS)
    target_compile_options(
            ${SNAP_TARGET_NAME}
            INTERFACE
            ${SNAP_warning_options}
    )
endif ()

# Enable AddressSanitizer if requested
if (SNAP_ENABLE_SANITIZER_BUILD)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
endif ()

# Setup project specific compile definitions that must continue with the interface
include(cmake/config/ProjectCompileDefinitions.cmake)

# Setup the version header for the library
set(SNAP_VERSION_MAJOR ${CMAKE_PROJECT_VERSION_MAJOR})
set(SNAP_VERSION_MINOR ${CMAKE_PROJECT_VERSION_MINOR})
set(SNAP_VERSION_PATCH ${CMAKE_PROJECT_VERSION_PATCH})
configure_file("${PROJECT_SOURCE_DIR}/cmake/in/version.hpp.in" "${PROJECT_BINARY_DIR}/include/SNAP/version.hpp")


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
