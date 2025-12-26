#ifndef SNP_TESTS_UNIT_TYPE_TRAITS_DEPENDENT_TYPE_HPP
#define SNP_TESTS_UNIT_TYPE_TRAITS_DEPENDENT_TYPE_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

SNAP_BEGIN_NAMESPACE
template <class T, bool> struct dependent_type : public T
{
};
SNAP_END_NAMESPACE

#endif // SNP_TESTS_UNIT_TYPE_TRAITS_DEPENDENT_TYPE_HPP
