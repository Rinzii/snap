#pragma once

#include "snap/bit/bit_width.hpp"

#include <limits>
#include <type_traits>

namespace snap
{
	template <typename T, typename... U> struct is_neither : std::integral_constant<bool, (!std::is_same_v<T, U> && ...)>
	{
	};
	template <typename T, typename... U> inline constexpr bool is_neither_v = is_neither<T, U...>::value;
} // namespace snap
