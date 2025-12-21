#pragma once

#include "snap/internal/abi_namespace.hpp"

#include <type_traits>

SNAP_BEGIN_NAMESPACE
template <class T> struct is_unbounded_array : std::false_type
	{
	};

	template <class T> struct is_unbounded_array<T[]> : std::true_type
	{
	};

	template <class T> constexpr inline bool is_unbounded_array_v = is_unbounded_array<T>::value;
SNAP_END_NAMESPACE
