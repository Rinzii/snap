#pragma once

#include "snap/functional/is_reference_wrapper.hpp"

namespace snap::internal {

    template <typename T>
    constexpr auto decay_reference_wrapper(T&& t) noexcept -> decltype(auto) {
        if constexpr (is_reference_wrapper<std::remove_cv_t<std::remove_reference_t<T>>>::value) {
            return std::forward<T>(t).get();
        } else {
            return std::forward<T>(t);
        }
    }

} // namespace snap::internal
