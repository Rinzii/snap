#pragma once

#include <type_traits>
#include <array>

#include "snap/bit/bit_cast.hpp"

namespace snap {

    template <class T,
              class = std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>>>
    SNAP_BITCAST_CONSTEXPR T byteswap(T value) noexcept {
        static_assert(std::has_unique_object_representations<T>::value,
                      "T may not have padding/trap bits");

        auto b = snap::bit_cast<std::array<std::byte, sizeof(T)>>(value);

        // reverse the representation bytes
        for (std::size_t i = 0, j = b.size() - 1; i < j; ++i, --j) {
            const std::byte tmp = b[i];
            b[i] = b[j];
            b[j] = tmp;
        }

        return snap::bit_cast<T>(b);
    }

} // namespace snap
