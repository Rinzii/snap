#ifndef SNP_INCLUDE_SNAP_META_ALWAYS_FALSE_HPP
#define SNP_INCLUDE_SNAP_META_ALWAYS_FALSE_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

SNAP_BEGIN_NAMESPACE
// Explanation of the purpose of this template.
// https://artificial-mind.net/blog/2020/10/03/always-false
// Not required in GCC 13.1+ and Clang 17.0.1+ but applied anyway for wider compatibility.
// See: https://cplusplus.github.io/CWG/issues/2518.html
template <typename...> inline constexpr bool always_false = false;
SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_META_ALWAYS_FALSE_HPP
