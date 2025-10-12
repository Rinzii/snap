#pragma once

#include "snap/internal/pp/has_builtin.hpp"

#include <limits>
#include <type_traits>

namespace snap
{
// Macro to allow simplified creation of specializations
// NOLINTBEGIN(bugprone-macro-parentheses)
#define INTERNAL_SNP_ADD_CTZ_SPECIALIZATION(FUNC, TYPE, BUILTIN)                                                                                               \
	template <> [[nodiscard]] constexpr int FUNC<TYPE>(TYPE value)                                                                                             \
	{                                                                                                                                                          \
		static_assert(std::is_unsigned_v<TYPE>);                                                                                                               \
		return value == 0 ? std::numeric_limits<TYPE>::digits : BUILTIN(value);                                                                                \
	}

	// NOLINTEND(bugprone-macro-parentheses)

#if SNAP_HAS_BUILTIN(__builtin_ctzg)
	/**
	 * @brief Returns the number of consecutive 0 bits in the value of x, starting from the least significant bit ("right").
	 * https://en.cppreference.com/w/cpp/numeric/countr_zero
	 */
	template <typename T> [[nodiscard]] constexpr std::enable_if_t<std::is_unsigned_v<T>, int> countr_zero(T value)
	{
		return __builtin_ctzg(value, std::numeric_limits<T>::digits); // NOLINT
	}
#else  // !SNAP_HAS_BUILTIN(__builtin_ctzg)
	/**
	 * @brief Returns the number of consecutive 0 bits in the value of x, starting from the least significant bit ("right").
	 * https://en.cppreference.com/w/cpp/numeric/countr_zero
	 */
	template <typename T> [[nodiscard]] constexpr std::enable_if_t<std::is_unsigned_v<T>, int> countr_zero(T value)
	{
		if (value == 0) { return std::numeric_limits<T>::digits; }
		if (value & 0x1) { return 0; }
		// Bisection method
		unsigned zero_bits = 0;
		unsigned shift	   = std::numeric_limits<T>::digits >> 1;
		T mask			   = std::numeric_limits<T>::max() >> shift;
		while (shift)
		{
			if ((value & mask) == 0)
			{
				value >>= shift;
				zero_bits |= shift;
			}
			shift >>= 1;
			mask >>= shift;
		}
		return zero_bits;
	}
#endif // SNAP_HAS_BUILTIN(__builtin_ctzg)

#if SNAP_HAS_BUILTIN(__builtin_ctzs)
	INTERNAL_SNP_ADD_CTZ_SPECIALIZATION(countr_zero, unsigned short, __builtin_ctzs)
#endif // SNAP_HAS_BUILTIN(__builtin_ctzs)
#if SNAP_HAS_BUILTIN(__builtin_ctz)
	INTERNAL_SNP_ADD_CTZ_SPECIALIZATION(countr_zero, unsigned int, __builtin_ctz)
#endif // SNAP_HAS_BUILTIN(__builtin_ctz)
#if SNAP_HAS_BUILTIN(__builtin_ctzl)
	INTERNAL_SNP_ADD_CTZ_SPECIALIZATION(countr_zero, unsigned long, __builtin_ctzl)
#endif // SNAP_HAS_BUILTIN(__builtin_ctzl)
#if SNAP_HAS_BUILTIN(__builtin_ctzll)
	INTERNAL_SNP_ADD_CTZ_SPECIALIZATION(countr_zero, unsigned long long, __builtin_ctzll)
#endif // SNAP_HAS_BUILTIN(__builtin_ctzll)

#undef INTERNAL_SNP_ADD_CTZ_SPECIALIZATION

	template <typename T> [[nodiscard]] constexpr std::enable_if_t<std::is_unsigned_v<T>, int> countr_one(T value)
	{
		return snap::countr_zero<T>(~value);
	}
} // namespace snap
