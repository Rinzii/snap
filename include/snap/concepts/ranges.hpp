#pragma once

#include "snap/internal/compat/std.hpp"

#if SNAP_HAS_CPP20
namespace snap::concepts {
    template<class R>
concept container_compatible_range =
    std::ranges::input_range<R> &&
    std::constructible_from<T, std::ranges::range_reference_t<R>>;
}
#endif // SNAP_HAS_CPP20
