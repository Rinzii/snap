#pragma once

#include "snap/internal/abi_namespace.hpp"

#include "snap/type_traits/is_relation.hpp"

#include <type_traits>

SNAP_BEGIN_NAMESPACE
template <class R, class T, class U> struct is_strict_weak_order : is_relation<R, T, U>
	{
	};

	template <class R, class T, class U> inline constexpr bool is_strict_weak_order_v = is_strict_weak_order<R, T, U>::value;

SNAP_END_NAMESPACE
