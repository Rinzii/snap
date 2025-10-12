#pragma once

#include "snap/type_traits/is_assignable_from.hpp"

#include <type_traits>

namespace snap
{
	template <class T> struct is_copyable : std::conjunction<std::is_copy_constructible<T>,
															 is_movable<T>,
															 is_assignable_from<T &, T &>,
															 is_assignable_from<T &, const T &>,
															 is_assignable_from<T &, const T>>
	{
	};

	template <class T> inline constexpr bool is_copyable_v = is_copyable<T>::value;
} // namespace snap
