#pragma once

#include "snap/type_traits/is_relation.hpp"

#include <type_traits>

namespace snap
{
	template <class R, class T, class U> struct is_strict_weak_order : is_relation<R, T, U>
	{
	};

	template <class R, class T, class U> inline constexpr bool is_strict_weak_order_v = is_strict_weak_order<R, T, U>::value;
} // namespace snap
