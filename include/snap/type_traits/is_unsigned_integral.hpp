#pragma once

#include "snap/internal/abi_namespace.hpp"
#include "snap/type_traits/is_signed_integral.hpp"

#include <type_traits>

SNAP_BEGIN_NAMESPACE
template <typename T> struct is_unsigned_integral
{
	static constexpr bool value = std::is_integral_v<T> && !is_signed_integral_v<T>;
};

template <typename T> inline constexpr bool is_unsigned_integral_v = is_unsigned_integral<T>::value;
SNAP_END_NAMESPACE
