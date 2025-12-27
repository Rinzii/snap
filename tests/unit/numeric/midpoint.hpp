#ifndef SNP_TESTS_UNIT_NUMERIC_MIDPOINT_HPP
#define SNP_TESTS_UNIT_NUMERIC_MIDPOINT_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <cstddef> // ptrdiff_t
#include <type_traits>

SNAP_BEGIN_NAMESPACE
// (1) Arithmetic midpoint: participates only if T is arithmetic and not bool
template <class T, class = std::enable_if_t<std::is_arithmetic_v<T> && !std::is_same_v<T, bool>>> constexpr T midpoint(T a, T b) noexcept
{
	if (std::is_floating_point_v<T>)
	{
		// At most one rounding, no overflow
		return a + (b - a) / T(2);
	}
	else
	{
		// Integer: no overflow, rounds toward a on odd sums
		using U = std::make_unsigned_t<T>;
		if (a < b)
		{
			const U diff = U(static_cast<U>(b) - static_cast<U>(a));
			return static_cast<T>(static_cast<U>(a) + (diff >> 1));
		}
		else if (a > b)
		{
			const U diff = U(static_cast<U>(a) - static_cast<U>(b));
			return static_cast<T>(static_cast<U>(a) - (diff >> 1));
		}
		else
		{
			return a;
		}
	}
}

// (2) Pointer midpoint: participates only if T is an object type
// UB unless a and b point into the same array object.
// Note: This simple form may not be portable on platforms where arrays can exceed PTRDIFF_MAX.
template <class T, class = std::enable_if_t<std::is_object_v<T>>> inline T* midpoint(T* a, T* b)
{
	return a + (b - a) / 2;
}

SNAP_END_NAMESPACE

#include <cstddef> // ptrdiff_t
#include <type_traits>

SNAP_BEGIN_NAMESPACE
// (1) Arithmetic midpoint: participates only if T is arithmetic and not bool
template <class T, class = std::enable_if_t<std::is_arithmetic_v<T> && !std::is_same_v<T, bool>>> constexpr T midpoint(T a, T b) noexcept
{
	if (std::is_floating_point_v<T>)
	{
		// At most one rounding, no overflow
		return a + (b - a) / T(2);
	}
	else
	{
		// Integer: no overflow, rounds toward a on odd sums
		using U = std::make_unsigned_t<T>;
		if (a < b)
		{
			const U diff = U(static_cast<U>(b) - static_cast<U>(a));
			return static_cast<T>(static_cast<U>(a) + (diff >> 1));
		}
		else if (a > b)
		{
			const U diff = U(static_cast<U>(a) - static_cast<U>(b));
			return static_cast<T>(static_cast<U>(a) - (diff >> 1));
		}
		else
		{
			return a;
		}
	}
}

// (2) Pointer midpoint: participates only if T is an object type
// UB unless a and b point into the same array object.
// Note: This simple form may not be portable on platforms where arrays can exceed PTRDIFF_MAX.
template <class T, class = std::enable_if_t<std::is_object_v<T>>> inline T* midpoint(T* a, T* b)
{
	return a + (b - a) / 2;
}

SNAP_END_NAMESPACE

#endif // SNP_TESTS_UNIT_NUMERIC_MIDPOINT_HPP
