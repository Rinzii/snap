#pragma once

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/bit/bit_cast.hpp"
#include "snap/type_traits/is_integer.hpp"

#include <array>
#include <type_traits>

SNAP_BEGIN_NAMESPACE
template <class T, std::enable_if_t<is_integer_v<T>, int> = 0> constexpr T byteswap(T value) noexcept
{
	static_assert(std::has_unique_object_representations_v<T>, "T may not have padding/trap bits");

	auto repr = SNAP_NAMESPACE::bit_cast<std::array<std::byte, sizeof(T)>>(value);

	for (std::size_t i = 0, n = repr.size(), half = n / 2; i < half; ++i) { std::swap(repr[i], repr[n - 1 - i]); }

	return SNAP_NAMESPACE::bit_cast<T>(repr);
}
SNAP_END_NAMESPACE
