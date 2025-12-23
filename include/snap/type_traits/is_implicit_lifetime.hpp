#pragma once

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <cstddef>
#include <type_traits>

SNAP_BEGIN_NAMESPACE
namespace detail
{

#if defined(__has_builtin)
	#if __has_builtin(__builtin_is_implicit_lifetime)
		#define SNAP_HAS_IL_BUILTIN 1
	#elif __has_builtin(__is_implicit_lifetime)
		#define SNAP_HAS_IL_BUILTIN 2
	#endif
#endif
#ifndef SNAP_HAS_IL_BUILTIN
	#define SNAP_HAS_IL_BUILTIN 0
#endif

	template <class T> struct il_base
#if SNAP_HAS_IL_BUILTIN == 1
		: std::bool_constant<__builtin_is_implicit_lifetime(std::remove_cv_t<T>)>
	{
	};
#elif SNAP_HAS_IL_BUILTIN == 2
		: std::bool_constant<__is_implicit_lifetime(std::remove_cv_t<T>)>
	{
	};
#else
		: std::bool_constant<std::is_scalar<std::remove_cv_t<T>>::value || std::is_trivially_copyable<std::remove_cv_t<T>>::value>
	{
	};
#endif

	template <class T> struct is_implicit_lifetime_impl : il_base<T>
	{
	};

	template <class T> struct is_implicit_lifetime_impl<T &> : std::false_type
	{
	};

	template <class T> struct is_implicit_lifetime_impl<T &&> : std::false_type
	{
	};

	template <class T> struct is_implicit_lifetime_impl<T[]> : is_implicit_lifetime_impl<T>
	{
	};

	template <class T, std::size_t N> struct is_implicit_lifetime_impl<T[N]> : is_implicit_lifetime_impl<T>
	{
	};

} // namespace detail

template <class T> struct is_implicit_lifetime : detail::is_implicit_lifetime_impl<std::remove_cv_t<T>>
{
};

template <class T> inline constexpr bool is_implicit_lifetime_v = is_implicit_lifetime<T>::value;

SNAP_END_NAMESPACE

#ifdef SNAP_HAS_IL_BUILTIN
	#undef SNAP_HAS_IL_BUILTIN
#endif
