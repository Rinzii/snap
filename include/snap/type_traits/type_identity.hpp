#ifndef SNP_INCLUDE_SNAP_TYPE_TRAITS_TYPE_IDENTITY_HPP
#define SNP_INCLUDE_SNAP_TYPE_TRAITS_TYPE_IDENTITY_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

SNAP_BEGIN_NAMESPACE

template <class Type> struct type_identity
{
	using type = Type;
};

template <class Type> using type_identity_t = typename type_identity<Type>::type;

SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_TYPE_TRAITS_TYPE_IDENTITY_HPP
