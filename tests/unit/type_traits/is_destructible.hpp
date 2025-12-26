#ifndef SNP_TESTS_UNIT_TYPE_TRAITS_IS_DESTRUCTIBLE_HPP
#define SNP_TESTS_UNIT_TYPE_TRAITS_IS_DESTRUCTIBLE_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <type_traits>

SNAP_BEGIN_NAMESPACE
template <class T> struct is_destructible : std::conjunction<std::is_object<T>, std::is_nothrow_destructible<T>>
{
};

template <class T> inline constexpr bool is_destructible_v = is_destructible<T>::value;
SNAP_END_NAMESPACE

#endif // SNP_TESTS_UNIT_TYPE_TRAITS_IS_DESTRUCTIBLE_HPP
