#pragma once

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <limits>
#include <type_traits>

SNAP_BEGIN_NAMESPACE
/**
 * \brief Computes the midpoint of the integers, floating-points, or pointers a and b.
 *
 * \tparam T Arithmetic type other than bool.
 * \param a First value.
 * \param b Second value.
 * \return Half of (a + b).
 *
 * \details
 * - Integer T: no overflow; if (a + b) is odd, the result rounds toward \p a.
 * - Floating T: computed so that at most one inexact operation occurs.
 *
 * \par Notes
 * This follows the intent and wording of std::midpoint (numeric.ops.midpoint).
 * For integers, we use the unsigned-distance method from the midpoint proposal (P0811).
 * For floating point, we use Marshall Clow's case split to avoid double rounding:
 *   - typical case: (a + b) / 2
 *   - otherwise: safely halve one or both arguments, then add
 *
 * \see P0811R3 (Strict provisions for std::midpoint)
 * \see CppCon 2019: Marshall Clow “std::midpoint? How Hard Could it Be?”
 */
template <class T, std::enable_if_t<std::is_arithmetic_v<T> && !std::is_same_v<T, bool>, int> = 0> constexpr T midpoint(T a, T b) noexcept
{
	if constexpr (std::is_floating_point_v<T>)
	{
		// At most one inexact operation.
		// Thresholds: lo = 2 * min(); hi = max() / 2
		using lim  = std::numeric_limits<T>;
		const T lo = lim::min() * T(2);
		const T hi = lim::max() / T(2);

		// abs
		const T aa = (a < T(0)) ? -a : a;
		const T bb = (b < T(0)) ? -b : b;

		if (aa <= hi && bb <= hi)
		{
			// Typical case: one rounding; no overflow.
			return (a + b) / T(2);
		}

		if (aa < lo)
		{
			// Not safe to halve a (may underflow toward subnormals) so halve b.
			return a + b / T(2);
		}

		if (bb < lo)
		{
			// Not safe to halve b so halve a.
			return a / T(2) + b;
		}

		// Both magnitudes large: halve both, then add.
		return a / T(2) + b / T(2);
	}
	else
	{
		// Integer: no overflow; rounds toward a on odd sums.
		using U = std::make_unsigned_t<T>;
		if (a < b)
		{
			const U diff = static_cast<U>(static_cast<U>(b) - static_cast<U>(a));
			return static_cast<T>(a + static_cast<T>(diff >> 1));
		}

		if (a > b)
		{
			const U diff = static_cast<U>(static_cast<U>(a) - static_cast<U>(b));
			return static_cast<T>(a - static_cast<T>(diff >> 1));
		}

		return a;
	}
}

/**
 * \brief Computes the midpoint of the integers, floating-points, or pointers a and b.
 *
 * \tparam T Object type.
 * \param a Pointer to element i.
 * \param b Pointer to element j.
 * \return a + (b - a) / 2 (division truncates toward zero).
 *
 * \pre \p a and \p b point to elements of the same array object (or one-past).
 *
 * \note Per the standard pointer arithmetic rules, (b - a) is well-formed
 * only when T is a complete object type and both pointers refer into the
 * same array object (or one-past). This enforces the mandate without
 * additional checks.
 * \see [expr.add]/2.2 in the C++ standard
 */
template <class T, std::enable_if_t<std::is_object_v<T>, int> = 0> constexpr T* midpoint(T* a, T* b) noexcept
{
	// Completeness and same-array are enforced by (b - a) itself.
	return a + (b - a) / 2;
}

SNAP_END_NAMESPACE
