#ifndef SNP_INCLUDE_SNAP_UTILITY_NONTYPE_HPP
#define SNP_INCLUDE_SNAP_UTILITY_NONTYPE_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

SNAP_BEGIN_NAMESPACE
template <auto V> struct nontype_t
{
	explicit nontype_t() = default;
};

template <auto V> inline constexpr nontype_t<V> nontype{};
SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_UTILITY_NONTYPE_HPP
