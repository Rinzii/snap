#pragma once

#include "snap/type_traits/is_assignable_from.hpp"

#include <type_traits>

namespace snap
{
	template <class T> struct is_movable : std::conjunction<std::is_object<T>, std::is_move_constructible<T>, is_assignable_from<T &, T>, std::is_swappable<T>>
	{
	};

	template <class T> inline constexpr bool is_movable_v = is_movable<T>::value;
} // namespace snap
