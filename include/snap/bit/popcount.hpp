#pragma once

#include "snap/bit/countl.hpp"
#include "snap/internal/abi_namespace.hpp"
#include "snap/internal/pp/has_builtin.hpp"

#include <cstdint>
#include <limits>
#include <type_traits>

SNAP_BEGIN_NAMESPACE
#if SNAP_HAS_BUILTIN(__builtin_popcountg)
template <typename T> [[nodiscard]] constexpr std::enable_if_t<std::is_unsigned_v<T>, int> popcount(T value)
{
	return __builtin_popcountg(value); // NOLINT
}
#else  // !SNAP_HAS_BUILTIN(__builtin_popcountg)
template <typename T> [[nodiscard]] constexpr std::enable_if_t<std::is_unsigned_v<T>, int> popcount(T value)
{
	int count = 0;
	for (int i = 0; i != std::numeric_limits<T>::digits; ++i)
	{
		if ((value >> i) & T{ 1 }) { ++count; }
	}
	return count;
}
#endif // SNAP_HAS_BUILTIN(__builtin_popcountg)

// NOLINTBEGIN(bugprone-macro-parentheses)
#define INTERNAL_SNP_ADD_POPCOUNT_SPECIALIZATION(FUNC, TYPE, BUILTIN)                                                                                          \
	template <> [[nodiscard]] constexpr int FUNC<TYPE>(TYPE value)                                                                                             \
	{                                                                                                                                                          \
		static_assert(std::is_unsigned_v<TYPE>);                                                                                                               \
		return BUILTIN(value);                                                                                                                                 \
	}
// NOLINTEND(bugprone-macro-parentheses)

// If the compiler has builtins for popcount, then create specializations that use the builtins.
#if SNAP_HAS_BUILTIN(__builtin_popcount)
INTERNAL_SNP_ADD_POPCOUNT_SPECIALIZATION(popcount, unsigned char, __builtin_popcount)
INTERNAL_SNP_ADD_POPCOUNT_SPECIALIZATION(popcount, unsigned short, __builtin_popcount)
INTERNAL_SNP_ADD_POPCOUNT_SPECIALIZATION(popcount, unsigned, __builtin_popcount)
#else
// If we don't have builtins, then provide optimizations for common types.
// All provided optimizations are based on the Hamming Weight algorithm except for unsigned char.
// https://en.wikipedia.org/wiki/Hamming_weight
constexpr int popcount(unsigned char n)
{
	// The algorithm is specific to 8-bit input, and avoids using 64-bit registers for code size.
	std::uint32_t r = static_cast<std::uint32_t>(static_cast<std::uint32_t>(n) * 0x08040201u);
	r				= static_cast<std::uint32_t>(((r >> 3) & 0x11111111u) * 0x11111111u) >> 28;
	return static_cast<int>(r);
}

constexpr int popcount(unsigned short n)
{
	// SWAR popcount
	n = static_cast<unsigned short>(n - ((n >> 1) & 0x5555u));
	n = static_cast<unsigned short>((n & 0x3333u) + ((n >> 2) & 0x3333u));
	n = static_cast<unsigned short>((n + (n >> 4)) & 0x0F0Fu);
	n = static_cast<unsigned short>(n + (n >> 8));
	return static_cast<int>(n & 0x1Fu);
}

constexpr int popcount(unsigned int n)
{
	// int can be 32 or 16 bits, so we need to check.
	if constexpr (std::numeric_limits<unsigned int>::digits == 32)
	{ // 32-bit int
		n = n - ((n >> 1) & 0x55555555u);
		n = (n & 0x33333333u) + ((n >> 2) & 0x33333333u);
		n = (n + (n >> 4)) & 0x0F0F0F0Fu;
		return static_cast<int>((n * 0x01010101u) >> 24);
	}
	else
	{ // 16-bit int
		n = n - ((n >> 1) & 0x5555u);
		n = (n & 0x3333u) + ((n >> 2) & 0x3333u);
		n = (n + (n >> 4)) & 0x0F0Fu;
		return static_cast<int>((n * 0x0101u) >> 16);
	}
}
#endif // SNAP_HAS_BUILTIN(__builtin_popcount)

#if SNAP_HAS_BUILTIN(__builtin_popcountl)
INTERNAL_SNP_ADD_POPCOUNT_SPECIALIZATION(popcount, unsigned long, __builtin_popcountl)
#else
constexpr int popcount(unsigned long n)
{
	// long can be 32 or 64 bits, so we need to check.
	if constexpr (std::numeric_limits<unsigned long>::digits == 32)
	{ // 32-bit long
		n = n - ((n >> 1) & 0x55555555ul);
		n = (n & 0x33333333ul) + ((n >> 2) & 0x33333333ul);
		n = (n + (n >> 4)) & 0x0F0F0F0Ful;
		return static_cast<int>((n * 0x01010101ul) >> 24);
	}
	else
	{ // 64-bit long
		n = n - ((n >> 1) & 0x5555555555555555ul);
		n = (n & 0x3333333333333333ul) + ((n >> 2) & 0x3333333333333333ul);
		n = (n + (n >> 4)) & 0x0F0F0F0F0F0F0F0Ful;
		return static_cast<int>((n * 0x0101010101010101ul) >> 56);
	}
}
#endif // SNAP_HAS_BUILTIN(__builtin_popcountl)

#if SNAP_HAS_BUILTIN(__builtin_popcountll)
INTERNAL_SNP_ADD_POPCOUNT_SPECIALIZATION(popcount, unsigned long long, __builtin_popcountll)
#else
constexpr int popcount(unsigned long long n)
{
	n = n - ((n >> 1) & 0x5555555555555555ull);
	n = (n & 0x3333333333333333ull) + ((n >> 2) & 0x3333333333333333ull);
	n = (n + (n >> 4)) & 0x0F0F0F0F0F0F0F0Full;
	return static_cast<int>((n * 0x0101010101010101ull) >> 56);
}
#endif // SNAP_HAS_BUILTIN(__builtin_popcountll)

#undef INTERNAL_SNP_ADD_POPCOUNT_SPECIALIZATION
SNAP_END_NAMESPACE
