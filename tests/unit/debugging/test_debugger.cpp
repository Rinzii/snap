#include <gtest/gtest.h>

#include "snap/debugging/breakpoint_if_debugging.hpp"
#include "snap/debugging/is_debugger_present.hpp"

TEST(Debugging, BreakpointHelperNoopsWithoutDebugger)
{
        EXPECT_FALSE(snap::is_debugger_present());
        EXPECT_NO_THROW(snap::breakpoint_if_debugging());
}
