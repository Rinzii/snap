#ifndef SNP_TESTS_UNIT_TYPE_TRAITS_IS_BOOLEAN_TESTABLE_HPP
#define SNP_TESTS_UNIT_TYPE_TRAITS_IS_BOOLEAN_TESTABLE_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <type_traits>

SNAP_BEGIN_NAMESPACE
template <class T> struct is_boolean_testable : std::is_convertible<T, bool>
{
};

template <class T> inline constexpr bool is_boolean_testable_v = is_boolean_testable<T>::value;
SNAP_END_NAMESPACE

#endif // SNP_TESTS_UNIT_TYPE_TRAITS_IS_BOOLEAN_TESTABLE_HPP
