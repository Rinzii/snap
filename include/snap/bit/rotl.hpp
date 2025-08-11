#pragma once

#include <type_traits>
#include <limits>
#include <cstdint> // For std::uint64_t

#include "snap/bit/rotr.hpp"

namespace snap {

    template <class T, std::enable_if_t<std::is_unsigned_v<T>, bool>  = true>
    constexpr T rotl(T t, int cnt) noexcept
    {
#if defined(_MSC_VER) && !defined(__clang__)
        // Allow for the use of compiler intrinsics if we are not being evaluated at compile time in msvc.
        if (!is_constant_evaluated())
        {
            // These func are not constexpr in msvc.
            if constexpr (std::is_same_v<T, unsigned int>) { return _rotl(t, cnt); }
            else if constexpr (std::is_same_v<T, std::uint64_t>) { return _rotl64(t, cnt); }
        }
#endif
        return rotr(t, -cnt);
    }


}
