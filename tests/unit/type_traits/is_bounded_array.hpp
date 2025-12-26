#ifndef SNP_TESTS_UNIT_TYPE_TRAITS_IS_BOUNDED_ARRAY_HPP
#define SNP_TESTS_UNIT_TYPE_TRAITS_IS_BOUNDED_ARRAY_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <type_traits>

SNAP_BEGIN_NAMESPACE
template <class T> struct is_bounded_array : std::false_type
{
};

template <class T, std::size_t N> struct is_bounded_array<T[N]> : std::true_type
{
};

template <class T> constexpr inline bool is_bounded_array_v = is_bounded_array<T>::value;
SNAP_END_NAMESPACE

#endif // SNP_TESTS_UNIT_TYPE_TRAITS_IS_BOUNDED_ARRAY_HPP
