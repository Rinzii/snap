#pragma once

#include "snap/type_traits/is_equality_comparable.hpp"
#include "snap/type_traits/is_semiregular.hpp"

#include <type_traits>

namespace snap
{
	template <class T> struct is_regular : std::conjunction<is_semiregular<T>, is_equality_comparable<T>>
	{
	};

	template <class T> inline constexpr bool is_regular_v = is_regular<T>::value;
} // namespace snap
