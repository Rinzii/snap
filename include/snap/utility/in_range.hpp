#ifndef SNP_INCLUDE_SNAP_UTILITY_IN_RANGE_HPP
#define SNP_INCLUDE_SNAP_UTILITY_IN_RANGE_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/utility/cmp.hpp"

#include <limits>

SNAP_BEGIN_NAMESPACE
template <class R, class T> constexpr bool in_range(T t) noexcept
{
	return SNAP_NAMESPACE::cmp_greater_equal(t, std::numeric_limits<R>::min()) && SNAP_NAMESPACE::cmp_less_equal(t, std::numeric_limits<R>::max());
}

SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_UTILITY_IN_RANGE_HPP
