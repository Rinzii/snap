#ifndef SNP_INCLUDE_SNAP_TYPE_TRAITS_IS_SIGNED_INTEGRAL_HPP
#define SNP_INCLUDE_SNAP_TYPE_TRAITS_IS_SIGNED_INTEGRAL_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <type_traits>

SNAP_BEGIN_NAMESPACE
template <typename T> struct is_signed_integral
{
	static constexpr bool value = std::is_integral_v<T> && std::is_signed_v<T>;
};

template <typename T> inline constexpr bool is_signed_integral_v = is_signed_integral<T>::value;
SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_TYPE_TRAITS_IS_SIGNED_INTEGRAL_HPP
