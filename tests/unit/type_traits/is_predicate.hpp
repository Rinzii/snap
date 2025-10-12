#pragma once

#include "snap/type_traits/is_boolean_testable.hpp"

#include <type_traits>

namespace snap
{
	template <class F, class... Args> struct is_predicate
		: std::conjunction<std::is_invocable<F, Args...>, is_boolean_testable<std::invoke_result_t<F, Args...>>>
	{
	};

	template <class F, class... Args> inline constexpr bool is_predicate_v = is_predicate<F, Args...>::value;
} // namespace snap
