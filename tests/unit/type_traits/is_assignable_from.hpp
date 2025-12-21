#pragma once

#include "snap/internal/abi_namespace.hpp"

#include <type_traits>

SNAP_BEGIN_NAMESPACE
template <class L, class R> struct is_assignable_from : std::conjunction<std::is_lvalue_reference<L>, std::is_assignable<L, R>>
	{
	};

	template <class L, class R> inline constexpr bool is_assignable_from_v = is_assignable_from<L, R>::value;
SNAP_END_NAMESPACE
