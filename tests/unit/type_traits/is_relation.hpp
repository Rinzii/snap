#pragma once

#include "snap/internal/abi_namespace.hpp"

#include "snap/type_traits/is_predicate.hpp"

#include <type_traits>

SNAP_BEGIN_NAMESPACE
template <class R, class T, class U> struct is_relation
		: std::conjunction<is_predicate<R, T, T>, is_predicate<R, U, U>, is_predicate<R, T, U>, is_predicate<R, U, T>>
	{
	};

	template <class R, class T, class U> inline constexpr bool is_relation_v = is_relation<R, T, U>::value;
SNAP_END_NAMESPACE
