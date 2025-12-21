#pragma once

#include "snap/internal/abi_namespace.hpp"

// C++17 snap::breakpoint_if_debugging
// Calls snap::breakpoint() only when snap::is_debugger_present() returns true.

#include "snap/debugging/breakpoint.hpp"
#include "snap/debugging/is_debugger_present.hpp"

SNAP_BEGIN_NAMESPACE
inline void breakpoint_if_debugging() noexcept
{
	if (is_debugger_present()) { breakpoint(); }
}

SNAP_END_NAMESPACE
