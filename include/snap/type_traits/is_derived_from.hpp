#pragma once

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <type_traits>

SNAP_BEGIN_NAMESPACE
template <class D, class B> struct is_derived_from : std::conjunction<std::is_base_of<B, D>, std::is_convertible<D *, B *>>
{
};

template <class D, class B> inline constexpr bool is_derived_from_v = is_derived_from<D, B>::value;
SNAP_END_NAMESPACE
