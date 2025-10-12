#pragma once

// C++17 snap::breakpoint_if_debugging
// Calls snap::breakpoint() only when snap::is_debugger_present() returns true.

#include "snap/debugging/breakpoint.hpp"
#include "snap/debugging/is_debugger_present.hpp"

namespace snap
{

	inline void breakpoint_if_debugging() noexcept
	{
		if (is_debugger_present()) { breakpoint(); }
	}

} // namespace snap
