#pragma once

#include <limits>
#include <type_traits>

#include "snap/bit/countl.hpp"
#include "snap/type_traits/is_integer.hpp"

namespace snap::internal {
    template <typename T>
    constexpr std::enable_if_t<is_unsigned_integer_v<T>, int> bit_log2(T value)
    {
        return std::numeric_limits<T>::digits - 1 - snap::countl_zero(value);
    }
} // namespace snap
