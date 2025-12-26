snap_add_headers(
        fixed_string.hpp
        inplace_vector.hpp
        numbers.hpp
        span.hpp
        version.hpp
)

if (CMAKE_CXX_STANDARD GREATER_EQUAL 20)
    snap_add_headers(
            concepts.hpp
    )
endif ()

add_subdirectory(bit)
add_subdirectory(concepts)
add_subdirectory(debugging)
add_subdirectory(expected)
add_subdirectory(functional)
add_subdirectory(internal)
add_subdirectory(iterator)
add_subdirectory(memory)
add_subdirectory(meta)
add_subdirectory(numeric)
add_subdirectory(simd)
add_subdirectory(stop_token)
add_subdirectory(thread)
add_subdirectory(type_traits)
add_subdirectory(utility)
