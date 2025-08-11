#pragma once

#include "snap/type_traits/is_char.hpp"

namespace snap {
    template <class T,
              std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T> && !is_char_v<T> && !std::is_same_v<T, bool>, bool>  = true>
    constexpr bool has_single_bit(T x) noexcept
    {
        return x && !(x & (x - 1));
    }
} // namespace snap
