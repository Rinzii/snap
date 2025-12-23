#pragma once

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/bit/countl.hpp"
#include "snap/type_traits/is_integer.hpp"

#include <limits>
#include <type_traits>

SNAP_BEGIN_NAMESPACE
namespace internal
{
	template <typename T> constexpr std::enable_if_t<is_unsigned_integer_v<T>, int> bit_log2(T value)
	{
		return std::numeric_limits<T>::digits - 1 - SNAP_NAMESPACE::countl_zero(value);
	}
} // namespace internal
SNAP_END_NAMESPACE
