#ifndef SNP_TESTS_UNIT_TYPE_TRAITS_TYPE_IDENTITY_HPP
#define SNP_TESTS_UNIT_TYPE_TRAITS_TYPE_IDENTITY_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

SNAP_BEGIN_NAMESPACE
template <typename Type> struct type_identity
{ // needed for c++17 support
	using type = Type;
};
SNAP_END_NAMESPACE

#endif // SNP_TESTS_UNIT_TYPE_TRAITS_TYPE_IDENTITY_HPP
