#ifndef SNP_TESTS_UNIT_TYPE_TRAITS_IS_SEMIREGULAR_HPP
#define SNP_TESTS_UNIT_TYPE_TRAITS_IS_SEMIREGULAR_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/type_traits/is_copyable.hpp"

#include <type_traits>

SNAP_BEGIN_NAMESPACE
template <class T> struct is_semiregular : std::conjunction<is_copyable<T>, std::is_default_constructible<T>>
{
};

template <class T> inline constexpr bool is_semiregular_v = is_semiregular<T>::value;
SNAP_END_NAMESPACE

#endif // SNP_TESTS_UNIT_TYPE_TRAITS_IS_SEMIREGULAR_HPP
