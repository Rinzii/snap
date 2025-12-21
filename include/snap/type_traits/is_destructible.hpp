#pragma once

#include "snap/internal/abi_namespace.hpp"

#include <type_traits>

SNAP_BEGIN_NAMESPACE
template <class T> struct is_destructible : std::conjunction<std::is_object<T>, std::is_nothrow_destructible<T>>
{
};

template <class T> inline constexpr bool is_destructible_v = is_destructible<T>::value;
SNAP_END_NAMESPACE
