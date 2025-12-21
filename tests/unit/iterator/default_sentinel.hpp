#pragma once

#include "snap/internal/abi_namespace.hpp"

SNAP_BEGIN_NAMESPACE
struct default_sentinel_t
	{
	};
	inline constexpr default_sentinel_t default_sentinel{};
SNAP_END_NAMESPACE
