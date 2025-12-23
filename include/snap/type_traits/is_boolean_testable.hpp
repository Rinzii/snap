#pragma once

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <type_traits>

SNAP_BEGIN_NAMESPACE
template <class T> struct is_boolean_testable : std::is_convertible<T, bool>
{
};

template <class T> inline constexpr bool is_boolean_testable_v = is_boolean_testable<T>::value;
SNAP_END_NAMESPACE
