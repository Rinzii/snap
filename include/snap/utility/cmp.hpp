#pragma once

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/type_traits/remove_cvref.hpp"

#include <cstdint>
#include <type_traits>

SNAP_BEGIN_NAMESPACE
namespace detail
{

	template <class T> struct is_forbidden_char_base : std::false_type
	{
	};

	template <> struct is_forbidden_char_base<char> : std::true_type
	{
	};

	template <> struct is_forbidden_char_base<signed char> : std::true_type
	{
	};

	template <> struct is_forbidden_char_base<unsigned char> : std::true_type
	{
	};

	template <> struct is_forbidden_char_base<wchar_t> : std::true_type
	{
	};

	template <> struct is_forbidden_char_base<char16_t> : std::true_type
	{
	};

	template <> struct is_forbidden_char_base<char32_t> : std::true_type
	{
	};

#if defined(__cpp_char8_t)
	template <> struct is_forbidden_char_base<char8_t> : std::true_type
	{
	};
#endif

	template <class T> inline constexpr bool is_forbidden_char_v = is_forbidden_char_base<remove_cvref_t<T>>::value;

	template <class T> inline constexpr bool is_allowed_cmp_int_v =
		std::is_integral_v<T> && !std::is_same_v<remove_cvref_t<T>, bool> && !is_forbidden_char_v<T>;

	template <class T, class U> using enable_cmp_t = std::enable_if_t<is_allowed_cmp_int_v<T> && is_allowed_cmp_int_v<U>, int>;

} // namespace detail

// cmp_equal
template <class T, class U, detail::enable_cmp_t<T, U> = 0> constexpr bool cmp_equal(T t, U u) noexcept
{
	if constexpr (std::is_signed_v<T> == std::is_signed_v<U>) { return t == u; }
	else if constexpr (std::is_signed_v<T>) { return t >= 0 && static_cast<std::make_unsigned_t<T>>(t) == u; }
	else { return u >= 0 && t == static_cast<std::make_unsigned_t<U>>(u); }
}

// cmp_not_equal
template <class T, class U, detail::enable_cmp_t<T, U> = 0> constexpr bool cmp_not_equal(T t, U u) noexcept
{
	return !cmp_equal(t, u);
}

// cmp_less
template <class T, class U, detail::enable_cmp_t<T, U> = 0> constexpr bool cmp_less(T t, U u) noexcept
{
	if constexpr (std::is_signed_v<T> == std::is_signed_v<U>) { return t < u; }
	else if constexpr (std::is_signed_v<T>) { return (t < 0) || (static_cast<std::make_unsigned_t<T>>(t) < u); }
	else { return (u >= 0) && (t < static_cast<std::make_unsigned_t<U>>(u)); }
}

// cmp_greater
template <class T, class U, detail::enable_cmp_t<T, U> = 0> constexpr bool cmp_greater(T t, U u) noexcept
{
	return cmp_less(u, t);
}

// cmp_less_equal
template <class T, class U, detail::enable_cmp_t<T, U> = 0> constexpr bool cmp_less_equal(T t, U u) noexcept
{
	return !cmp_less(u, t);
}

// cmp_greater_equal
template <class T, class U, detail::enable_cmp_t<T, U> = 0> constexpr bool cmp_greater_equal(T t, U u) noexcept
{
	return !cmp_less(t, u);
}

SNAP_END_NAMESPACE
