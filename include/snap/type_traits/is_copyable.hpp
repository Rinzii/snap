#ifndef SNP_INCLUDE_SNAP_TYPE_TRAITS_IS_COPYABLE_HPP
#define SNP_INCLUDE_SNAP_TYPE_TRAITS_IS_COPYABLE_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/type_traits/is_assignable_from.hpp"
#include "snap/type_traits/is_movable.hpp"

#include <type_traits>

SNAP_BEGIN_NAMESPACE
template <class T> struct is_copyable : std::conjunction<std::is_copy_constructible<T>,
														 is_movable<T>,
														 is_assignable_from<T &, T &>,
														 is_assignable_from<T &, const T &>,
														 is_assignable_from<T &, const T>>
{
};

template <class T> inline constexpr bool is_copyable_v = is_copyable<T>::value;
SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_TYPE_TRAITS_IS_COPYABLE_HPP
