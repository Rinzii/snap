if (NOT DEFINED SNAP_ROOT_DIR)
    message(FATAL_ERROR "SNAP_ROOT_DIR is not defined. Did you forget to include the main CMakeLists.txt?")
endif ()

if (NOT SNAP_DISABLE_CMAKE_FEATURE_CHECKS)
    include(${SNAP_ROOT_DIR}/cmake/features/CheckAllSupportedBuiltinFeatures.cmake)

    if (SNAP_ENABLE_RUNTIME_SIMD)
        include(${SNAP_ROOT_DIR}/cmake/features/CheckAllSupportedSimdFeatures.cmake)
    endif ()
else ()
    message(STATUS "CCMath: Requested disabled cmake feature checking. Be warned this might have undefined issues in the library.")
endif ()
