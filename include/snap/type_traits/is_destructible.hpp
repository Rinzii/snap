#pragma once

#include <type_traits>

namespace snap
{
	template <class T> struct is_destructible : std::conjunction<std::is_object<T>, std::is_nothrow_destructible<T>>
	{
	};

	template <class T> inline constexpr bool is_destructible_v = is_destructible<T>::value;
} // namespace snap
