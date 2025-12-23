#pragma once

/**
 * @file
 * @brief Portable C++17 trait to detect implicit-lifetime types with a conservative fallback.
 *
 * @details
 * This header provides `SNAP_NAMESPACE::is_implicit_lifetime<T>` and the convenience
 * variable `SNAP_NAMESPACE::is_implicit_lifetime_v<T>`. When the compiler exposes a
 * builtin (`__builtin_is_implicit_lifetime` or `__is_implicit_lifetime`),
 * the trait is exact. Otherwise it falls back to a conservative rule:
 *
 * - `true` for scalars and trivially copyable types
 * - `true` for arrays thereof (any rank)
 * - `false` for references
 *
 * The fallback prefers reporting `false` in ambiguous cases (no false positives).
 */

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <cstddef>
#include <type_traits>

SNAP_BEGIN_NAMESPACE
namespace detail
{

/**
 * @def SNAP_HAS_IL_BUILTIN
 * @brief Internal feature test for compiler implicit-lifetime builtins.
 *
 * Values:
 * - `0` : No builtin available; use the conservative fallback.
 * - `1` : `__builtin_is_implicit_lifetime` is available (Clang/GCC).
 * - `2` : `__is_implicit_lifetime` is available (alternate builtin).
 *
 * @note This macro is for internal use only and is undefined at the end
 *       of the header when it was introduced here.
 */
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

	/**
	 * @brief Base check that determines implicit-lifetime without recursing
	 *        into the public trait during its own definition.
	 *
	 * @tparam T The type being tested.
	 *
	 * @details
	 * - If a compiler builtin is available, delegates to it (exact behavior).
	 * - Otherwise, applies the conservative C++17 approximation:
	 *   `is_scalar<remove_cv_t<T>> || is_trivially_copyable<remove_cv_t<T>>`.
	 */
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

	/**
	 * @brief Implementation detail that rejects references and peels arrays,
	 *        then consults @ref il_base for the element/base type.
	 *
	 * @tparam T The type being tested.
	 */
	template <class T> struct is_implicit_lifetime_impl : il_base<T>
	{
	};

	/** @brief References are never implicit-lifetime. */
	template <class T> struct is_implicit_lifetime_impl<T &> : std::false_type
	{
	};

	/** @brief Rvalue references are never implicit-lifetime. */
	template <class T> struct is_implicit_lifetime_impl<T &&> : std::false_type
	{
	};

	/**
	 * @brief For arrays of unknown bound, defer to the element type.
	 * @tparam T Element type.
	 */
	template <class T> struct is_implicit_lifetime_impl<T[]> : is_implicit_lifetime_impl<T>
	{
	};

	/**
	 * @brief For arrays of known bound, defer to the element type.
	 * @tparam T Element type.
	 * @tparam N Array bound.
	 */
	template <class T, std::size_t N> struct is_implicit_lifetime_impl<T[N]> : is_implicit_lifetime_impl<T>
	{
	};

} // namespace detail

/**
 * @brief Trait that determines whether `T` is an implicit-lifetime type.
 *
 * @tparam T The type being tested.
 *
 * @details
 * Applies compiler builtins when available. Otherwise uses a conservative
 * C++17-safe approximation. Arrays are handled by examining their element
 * type; references are reported as `false`.
 */
template <class T> struct is_implicit_lifetime : detail::is_implicit_lifetime_impl<std::remove_cv_t<T>>
{
};

/**
 * @brief Convenience variable template for @ref is_implicit_lifetime.
 * @tparam T The type being tested.
 */
template <class T> inline constexpr bool is_implicit_lifetime_v = is_implicit_lifetime<T>::value;

SNAP_END_NAMESPACE

#ifdef SNAP_HAS_IL_BUILTIN
	#undef SNAP_HAS_IL_BUILTIN
#endif
