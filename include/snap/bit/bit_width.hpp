#pragma once

#include <type_traits>

#include <snap/type_traits/is_integer.hpp>
#include "snap/internal/helpers/bit_log2.hpp"

namespace snap {
    template <typename T>
    constexpr std::enable_if_t<is_unsigned_integer_v<T>, int>  bit_width(T t)
    {
        return t == 0 ? 0 : internal::bit_log2(t) + 1;
    }
} // namespace snap
