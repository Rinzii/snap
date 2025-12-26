#ifndef SNP_INCLUDE_SNAP_UTILITY_UNREACHABLE_HPP
#define SNP_INCLUDE_SNAP_UTILITY_UNREACHABLE_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

SNAP_BEGIN_NAMESPACE

[[noreturn]] void unreachable() noexcept;

SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_UTILITY_UNREACHABLE_HPP
