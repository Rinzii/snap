#pragma once

#include "snap/internal/abi_namespace.hpp"

SNAP_BEGIN_NAMESPACE
template <typename Type> struct type_identity
{
	using type = Type;
};
SNAP_END_NAMESPACE
