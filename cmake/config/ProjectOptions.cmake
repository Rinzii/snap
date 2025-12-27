# -- Internal variables used by snap --

set(SNAP_DESIRED_CXX_STANDARD cxx_std_17 CACHE STRING "")
mark_as_advanced(SNAP_DESIRED_CXX_STANDARD)

if (NOT DEFINED SNAP_ROOT_DIR)
    set(SNAP_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif ()
mark_as_advanced(SNAP_ROOT_DIR)

# Helper to declare boolean options whose defaults depend on whether snap is the
# top-level project. This lets consumers opt-out while our own builds still get
# the intended defaults.
if (NOT COMMAND snap_option)
    macro(snap_option NAME DESCRIPTION DEFAULT_TOP_LEVEL DEFAULT_SUBPROJECT)
        set(_snap_option_default ${DEFAULT_SUBPROJECT})
        if (SNAP_PROJECT_IS_TOP_LEVEL)
            set(_snap_option_default ${DEFAULT_TOP_LEVEL})
        endif ()
        option(${NAME} "${DESCRIPTION}" ${_snap_option_default})
        unset(_snap_option_default)
    endmacro()
endif ()

if (NOT COMMAND snap_require_fetchcontent_allowed)
    function(snap_require_fetchcontent_allowed feature_name)
        if (SNAP_DISABLE_FETCHCONTENT)
            message(FATAL_ERROR
                    "SNAP_DISABLE_FETCHCONTENT=ON. Provide ${feature_name} yourself or turn the toggle OFF to allow FetchContent.")
        endif ()
    endfunction()
endif ()
