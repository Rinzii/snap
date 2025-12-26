if (CMAKE_CXX_STANDARD GREATER_EQUAL 20)
    snap_add_headers(
            boolean_testable.hpp
            container_compatible_range.hpp
            hashable.hpp
            integer.hpp
            istream_extractable.hpp
            operators.hpp
            ostream_insertable.hpp
            pair_like.hpp
            tuple_like.hpp
            variant_like.hpp
    )
endif ()
