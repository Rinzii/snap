#pragma once

// C++17 SNAP_NAMESPACE::breakpoint_if_debugging
// Calls SNAP_NAMESPACE::breakpoint() only when SNAP_NAMESPACE::is_debugger_present() returns true.

// Must be included first
#include "snap/internal/abi_namespace.hpp"

SNAP_BEGIN_NAMESPACE

void breakpoint_if_debugging() noexcept;

SNAP_END_NAMESPACE
