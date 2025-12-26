#ifndef SNP_INCLUDE_SNAP_ITERATOR_DEFAULT_SENTINEL_HPP
#define SNP_INCLUDE_SNAP_ITERATOR_DEFAULT_SENTINEL_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

SNAP_BEGIN_NAMESPACE
struct default_sentinel_t
{
};

inline constexpr default_sentinel_t default_sentinel{};
SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_ITERATOR_DEFAULT_SENTINEL_HPP
