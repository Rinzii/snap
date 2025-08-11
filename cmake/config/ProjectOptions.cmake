# -- Internal variables used by CCMath --

set(SNAP_DESIRED_CXX_STANDARD cxx_std_17 CACHE STRING "")
mark_as_advanced(SNAP_DESIRED_CXX_STANDARD)

if (NOT DEFINED SNAP_ROOT_DIR)
    set(SNAP_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif ()
mark_as_advanced(SNAP_ROOT_DIR)
