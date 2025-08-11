#pragma once

#include <limits>

#include "snap/utility/cmp.hpp"

namespace snap {

    template<class R, class T>
    constexpr bool in_range(T t) noexcept
    {
        return snap::cmp_greater_equal(t, std::numeric_limits<R>::min()) &&
            snap::cmp_less_equal(t, std::numeric_limits<R>::max());
    }

} // namespace snap
