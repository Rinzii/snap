#pragma once

#include <type_traits>

namespace snap
{
	template <typename T> struct is_signed_integral
	{
		static constexpr bool value = std::is_integral_v<T> && std::is_signed_v<T>;
	};

	template <typename T> inline constexpr bool is_signed_integral_v = is_signed_integral<T>::value;
} // namespace snap
