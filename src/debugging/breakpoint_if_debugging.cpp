#include "snap/debugging/breakpoint_if_debugging.hpp"

#include "snap/debugging/breakpoint.hpp"
#include "snap/debugging/is_debugger_present.hpp"

SNAP_BEGIN_NAMESPACE

void breakpoint_if_debugging() noexcept
{
	if (is_debugger_present()) { breakpoint(); }
}

SNAP_END_NAMESPACE
