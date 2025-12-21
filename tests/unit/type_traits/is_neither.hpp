#pragma once

#include "snap/internal/abi_namespace.hpp"

#include "snap/bit/bit_width.hpp"

#include <limits>
#include <type_traits>

SNAP_BEGIN_NAMESPACE
template <typename T, typename... U> struct is_neither : std::integral_constant<bool, (!std::is_same_v<T, U> && ...)>
	{
	};
	template <typename T, typename... U> inline constexpr bool is_neither_v = is_neither<T, U...>::value;
SNAP_END_NAMESPACE
