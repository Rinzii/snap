#ifndef SNP_INCLUDE_SNAP_DEBUGGING_IS_DEBUGGER_PRESENT_HPP
#define SNP_INCLUDE_SNAP_DEBUGGING_IS_DEBUGGER_PRESENT_HPP

// TODO: Needs more testing on additional BSD variants / versions.

// Must be included first
#include "snap/internal/abi_namespace.hpp"

SNAP_BEGIN_NAMESPACE

bool is_debugger_present() noexcept;

SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_DEBUGGING_IS_DEBUGGER_PRESENT_HPP
