#pragma once

#include "snap/type_traits/is_copyable.hpp"

#include <type_traits>

namespace snap
{
	template <class T> struct is_semiregular : std::conjunction<is_copyable<T>, std::is_default_constructible<T>>
	{
	};

	template <class T> inline constexpr bool is_semiregular_v = is_semiregular<T>::value;
} // namespace snap
