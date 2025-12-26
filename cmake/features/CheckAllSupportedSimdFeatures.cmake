if (NOT DEFINED SNAP_ROOT_DIR)
    message(FATAL_ERROR "SNAP_ROOT_DIR is not defined. Did you forget to include the main CMakeLists.txt?")
endif ()

include(${SNAP_ROOT_DIR}/cmake/features/simd/CheckFMASupport.cmake)

if (NOT SNAP_DISABLE_SVML_USAGE)
    include(${SNAP_ROOT_DIR}/cmake/features/simd/CheckSVMLSupport.cmake)
endif ()
