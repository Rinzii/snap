#pragma once

#include <type_traits>

namespace snap
{
	template <class T> struct is_boolean_testable : std::is_convertible<T, bool>
	{
	};

	template <class T> inline constexpr bool is_boolean_testable_v = is_boolean_testable<T>::value;
} // namespace snap
