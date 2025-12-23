# Helper functions for adding headers to the interfaces of ccmath

function(snap_add_headers)
    foreach (header IN LISTS ARGV)
        target_sources(
                ${SNAP_TARGET_NAME}
                PUBLIC
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${header}>"
        )
    endforeach ()
endfunction()

function(snap_add_sources)
    foreach (src IN LISTS ARGV)
        target_sources(
                ${SNAP_TARGET_NAME}
                PRIVATE
                "${CMAKE_CURRENT_SOURCE_DIR}/${src}"
        )
    endforeach ()
endfunction()

# TODO: Verify this is correct
function(snap_add_target_interface target)
    foreach (header IN LISTS ARGV)
        if (NOT header STREQUAL ${target})
            target_sources(
                    ${target}
                    PUBLIC
                    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${header}>"
            )
        endif ()
    endforeach ()
endfunction()
