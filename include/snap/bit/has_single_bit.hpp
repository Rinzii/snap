#ifndef SNP_INCLUDE_SNAP_BIT_HAS_SINGLE_BIT_HPP
#define SNP_INCLUDE_SNAP_BIT_HAS_SINGLE_BIT_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/type_traits/is_char.hpp"

SNAP_BEGIN_NAMESPACE
template <class T, std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T> && !is_char_v<T> && !std::is_same_v<T, bool>, bool> = true>
constexpr bool has_single_bit(T x) noexcept
{
	return x && !(x & (x - 1));
}
SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_BIT_HAS_SINGLE_BIT_HPP
