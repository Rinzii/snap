#pragma once

// TODO: Needs more testing on additional BSD variants / versions.

// Must be included first
#include "snap/internal/abi_namespace.hpp"

SNAP_BEGIN_NAMESPACE

// defined out-of-line to avoid redundant platform headers for every user include
bool is_debugger_present() noexcept;

SNAP_END_NAMESPACE
