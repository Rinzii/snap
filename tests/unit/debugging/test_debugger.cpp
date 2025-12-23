#include "snap/debugging/breakpoint_if_debugging.hpp"
#include "snap/debugging/is_debugger_present.hpp"

#include <gtest/gtest.h>

TEST(Debugging, BreakpointHelperNoopsWithoutDebugger)
{
	EXPECT_FALSE(SNAP_NAMESPACE::is_debugger_present());
	EXPECT_NO_THROW(SNAP_NAMESPACE::breakpoint_if_debugging());
}
