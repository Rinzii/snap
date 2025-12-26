#ifndef SNP_INCLUDE_SNAP_BIT_BIT_WIDTH_HPP
#define SNP_INCLUDE_SNAP_BIT_BIT_WIDTH_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/internal/helpers/bit_log2.hpp"
#include "snap/type_traits/is_integer.hpp"

#include <type_traits>

SNAP_BEGIN_NAMESPACE
template <typename T> constexpr auto bit_width(T t) noexcept -> std::enable_if_t<is_unsigned_integer_v<T>, int>
{
	return t == 0 ? 0 : internal::bit_log2(t) + 1;
}
SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_BIT_BIT_WIDTH_HPP
