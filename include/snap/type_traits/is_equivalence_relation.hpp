#pragma once

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/type_traits/is_relation.hpp"

#include <type_traits>

SNAP_BEGIN_NAMESPACE
template <class R, class T, class U> struct is_equivalence_relation : is_relation<R, T, U>
{
};

template <class R, class T, class U> inline constexpr bool is_equivalence_relation_v = is_equivalence_relation<R, T, U>::value;

SNAP_END_NAMESPACE
