#pragma once

#include "snap/internal/compat/std.hpp"

namespace snap {
#if SNAP_HAS_CPP20
	// C++20 constexpr friendly check
    template<class F, int = (F{}(), 0)>
    constexpr bool is_constexpr_friendly(F) { return true; }
    constexpr bool is_constexpr_friendly(...) { return false; }
#endif
}
