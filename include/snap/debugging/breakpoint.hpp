#pragma once

// TODO: Should be complete but needs more testing on other environments.

// Must be included first
#include "snap/internal/abi_namespace.hpp"

SNAP_BEGIN_NAMESPACE

// lives in src/debugging; keep header tiny for dependents
void breakpoint() noexcept;

SNAP_END_NAMESPACE
