#pragma once

#include "snap/type_traits/is_signed_integral.hpp"

#include <type_traits>

namespace snap
{
	template <typename T> struct is_unsigned_integral
	{
		static constexpr bool value = std::is_integral_v<T> && !is_signed_integral_v<T>;
	};

	template <typename T> inline constexpr bool is_unsigned_integral_v = is_unsigned_integral<T>::value;
} // namespace snap
