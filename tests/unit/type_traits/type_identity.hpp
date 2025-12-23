#pragma once

// Must be included first
#include "snap/internal/abi_namespace.hpp"

SNAP_BEGIN_NAMESPACE
template <typename Type> struct type_identity
{ // needed for c++17 support
	using type = Type;
};
SNAP_END_NAMESPACE
