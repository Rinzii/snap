#ifndef SNP_TESTS_UNIT_TYPE_TRAITS_IS_SPECIALIZATION_OF_HPP
#define SNP_TESTS_UNIT_TYPE_TRAITS_IS_SPECIALIZATION_OF_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <type_traits>

// This implementation still suffers from the same limitations as std::is_specialization_of

SNAP_BEGIN_NAMESPACE
template <class T, template <class...> class Primary> struct is_specialization_of : std::false_type;

template <template <class...> class Primary, class... Args> struct is_specialization_of<Primary<Args...>, Primary> : std::true_type;

template <class T, template <class...> class Primary> inline constexpr bool is_specialization_of_v = is_specialization_of<T, Primary>::value;
SNAP_END_NAMESPACE

#endif // SNP_TESTS_UNIT_TYPE_TRAITS_IS_SPECIALIZATION_OF_HPP
