#ifndef SNP_INCLUDE_SNAP_FUNCTIONAL_IS_REFERENCE_WRAPPER_HPP
#define SNP_INCLUDE_SNAP_FUNCTIONAL_IS_REFERENCE_WRAPPER_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <functional>
#include <type_traits>

SNAP_BEGIN_NAMESPACE
template <typename> struct is_reference_wrapper : std::false_type
{
};

template <typename T> struct is_reference_wrapper<std::reference_wrapper<T>> : std::true_type
{
};
SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_FUNCTIONAL_IS_REFERENCE_WRAPPER_HPP
