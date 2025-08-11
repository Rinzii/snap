#pragma once

#include <type_traits>
#include <limits>

#include "snap/type_traits/is_integer.hpp"
#include "snap/bit/countl.hpp"

namespace snap {

    template <typename T>
    [[nodiscard]] constexpr std::enable_if_t<is_unsigned_integer_v<T>, T> bit_ceil(T x) noexcept
    {
        if (x < 2) { return 1; }
        const unsigned n = std::numeric_limits<T>::digits - snap::countl_zero(static_cast<T>(x - 1U));
        assert(n != std::numeric_limits<T>::digits && "n is a bad input for bit_ceil");

        if constexpr (sizeof(T) >= sizeof(unsigned)) {
            return T{1} << n;
        }
        else {
            const unsigned extra = std::numeric_limits<unsigned>::digits - std::numeric_limits<T>::digits;
            const unsigned retVal = 1u << (n + extra);
            return static_cast<T>(retVal >> extra);
        }
    }

} // namespace snap
