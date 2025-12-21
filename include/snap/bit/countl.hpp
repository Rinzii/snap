#pragma once

#include "snap/internal/abi_namespace.hpp"
#include "snap/internal/pp/has_builtin.hpp"
#include "snap/type_traits/is_integer.hpp"

#include <limits>
#include <type_traits>

SNAP_BEGIN_NAMESPACE
// Macro to allow simplified creation of specializations
// NOLINTBEGIN(bugprone-macro-parentheses)
#define INTERNAL_SNP_ADD_CTZ_SPECIALIZATION(FUNC, TYPE, BUILTIN)                                                                                               \
	template <> [[nodiscard]] constexpr int FUNC<TYPE>(TYPE value)                                                                                             \
	{                                                                                                                                                          \
		static_assert(std::is_unsigned_v<TYPE>);                                                                                                               \
		return value == 0 ? std::numeric_limits<TYPE>::digits : BUILTIN(value);                                                                                \
	}
// NOLINTEND(bugprone-macro-parentheses)

#if SNAP_HAS_BUILTIN(__builtin_clzg)
template <typename T> [[nodiscard]] constexpr std::enable_if_t<std::is_unsigned_v<T>, int> countl_zero(T value)
{
	return __builtin_clzg(value, std::numeric_limits<T>::digits); // NOLINT
}
#else  // !SNAP_HAS_BUILTIN(__builtin_clzg)
template <typename T> [[nodiscard]] constexpr std::enable_if_t<std::is_unsigned_v<T>, int> countl_zero(T value)
{
	if (value == 0) { return std::numeric_limits<T>::digits; }
	// Bisection method
	unsigned zero_bits = 0;
	for (unsigned shift = std::numeric_limits<T>::digits >> 1; shift; shift >>= 1)
	{
		T tmp = value >> shift;
		if (tmp) { value = tmp; }
		else { zero_bits |= shift; }
	}
	return zero_bits;
}
#endif // SNAP_HAS_BUILTIN(__builtin_clzg)

#if SNAP_HAS_BUILTIN(__builtin_clzs)
INTERNAL_SNP_ADD_CTZ_SPECIALIZATION(countl_zero, unsigned short, __builtin_clzs)
#endif // SNAP_HAS_BUILTIN(__builtin_clzs)
#if SNAP_HAS_BUILTIN(__builtin_clz)
INTERNAL_SNP_ADD_CTZ_SPECIALIZATION(countl_zero, unsigned int, __builtin_clz)
#endif // SNAP_HAS_BUILTIN(__builtin_clz)
#if SNAP_HAS_BUILTIN(__builtin_clzl)
INTERNAL_SNP_ADD_CTZ_SPECIALIZATION(countl_zero, unsigned long, __builtin_clzl)
#endif // SNAP_HAS_BUILTIN(__builtin_clzl)
#if SNAP_HAS_BUILTIN(__builtin_clzll)
INTERNAL_SNP_ADD_CTZ_SPECIALIZATION(countl_zero, unsigned long long, __builtin_clzll)
#endif // SNAP_HAS_BUILTIN(__builtin_clzll)

#undef INTERNAL_SNP_ADD_CTZ_SPECIALIZATION

template <typename T, std::enable_if_t<is_unsigned_integer_v<T>, bool> = true>
[[nodiscard]] constexpr std::enable_if_t<std::is_unsigned_v<T>, int> countl_one(T value)
{
	return value != std::numeric_limits<T>::max() ? snap::countl_zero(static_cast<T>(~value)) : std::numeric_limits<T>::digits;
}
SNAP_END_NAMESPACE
