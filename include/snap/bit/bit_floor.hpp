#ifndef SNP_INCLUDE_SNAP_BIT_BIT_FLOOR_HPP
#define SNP_INCLUDE_SNAP_BIT_BIT_FLOOR_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/bit/bit_width.hpp"
#include "snap/type_traits/is_neither.hpp"

#include <type_traits>

SNAP_BEGIN_NAMESPACE
namespace detail
{
	template <class T> inline constexpr bool is_not_invalid_for_bit_floor = std::is_unsigned_v<std::remove_cv_t<T>> && is_neither_v<std::remove_cv_t<T>,
																																	bool,
																																	char,
#ifdef __cpp_char8_t
																																	char8_t,
#endif // __cpp_char8_t
																																	char16_t,
																																	char32_t,
																																	wchar_t>;
} // namespace detail

template <class T, std::enable_if_t<detail::is_not_invalid_for_bit_floor<T>, bool> = true> constexpr T bit_floor(T x) noexcept
{
	if (x != 0) { return static_cast<T>(T{ 1 } << (bit_width(x) - 1)); }
	return T{ 0 };
}
SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_BIT_BIT_FLOOR_HPP
