#pragma once

// C++17 snap::breakpoint_if_debugging
// Calls snap::breakpoint() only when snap::is_debugger_present() returns true.

#include "snap/debugging/is_debugger_present.hpp"
#include "snap/debugging/breakpoint.hpp"

namespace snap {

    inline void breakpoint_if_debugging() noexcept {
        if (snap::is_debugger_present()) {
            snap::breakpoint();
        }
    }

} // namespace snap
