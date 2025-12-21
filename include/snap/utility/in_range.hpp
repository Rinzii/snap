#pragma once

#include "snap/internal/abi_namespace.hpp"
#include "snap/utility/cmp.hpp"

#include <limits>

SNAP_BEGIN_NAMESPACE
template <class R, class T> constexpr bool in_range(T t) noexcept
{
	return snap::cmp_greater_equal(t, std::numeric_limits<R>::min()) && snap::cmp_less_equal(t, std::numeric_limits<R>::max());
}

SNAP_END_NAMESPACE
