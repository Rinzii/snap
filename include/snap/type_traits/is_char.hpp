#pragma once

#include "snap/internal/abi_namespace.hpp"

#include <type_traits>

SNAP_BEGIN_NAMESPACE
namespace detail
{
	template <class> struct is_char_impl : std::false_type
	{
	};
	template <> struct is_char_impl<char> : std::true_type
	{
	};
	template <> struct is_char_impl<signed char> : std::true_type
	{
	};
	template <> struct is_char_impl<unsigned char> : std::true_type
	{
	};
	template <> struct is_char_impl<wchar_t> : std::true_type
	{
	};
	template <> struct is_char_impl<char16_t> : std::true_type
	{
	};
	template <> struct is_char_impl<char32_t> : std::true_type
	{
	};
#if defined(__cpp_char8_t)
	template <> struct is_char_impl<char8_t> : std::true_type
	{
	};
#endif
} // namespace detail

template <class T> struct is_char : detail::is_char_impl<typename std::remove_cv<typename std::remove_reference<T>::type>::type>
{
};

template <class T> inline constexpr bool is_char_v = is_char<T>::value;

SNAP_END_NAMESPACE
