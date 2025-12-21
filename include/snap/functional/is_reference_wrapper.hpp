#pragma once

#include "snap/internal/abi_namespace.hpp"

#include <functional>
#include <type_traits>

SNAP_BEGIN_NAMESPACE
template <typename T> struct is_reference_wrapper : std::false_type
	{
	};

	template <typename U> struct is_reference_wrapper<std::reference_wrapper<U>> : std::true_type
	{
	};
SNAP_END_NAMESPACE
