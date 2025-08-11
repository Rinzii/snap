#pragma once

#include <type_traits>
#include <limits>

#include "snap/bit/bit_width.hpp"
#include "snap/type_traits/is_neither.hpp"

namespace snap {

    template <typename T,
              typename = std::enable_if_t<
                  std::is_unsigned<T> &&
                  is_neither_v<
                      T, bool, char,
    #if defined(__cpp_char8_t)
                      char8_t,
    #endif
                      char16_t, char32_t, wchar_t
                  >
              >>
    constexpr T bit_floor(T x) noexcept
    {
        if (x != 0) {
            // highest power-of-two <= x
            return static_cast<T>(T{1} << (bit_width(x) - 1));
        }
        return T{0};
    }

} // namespace snap
