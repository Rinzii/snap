// snap/concepts/variant_like.hpp
#ifndef SNP_INCLUDE_SNAP_CONCEPTS_VARIANT_LIKE_HPP
#define SNP_INCLUDE_SNAP_CONCEPTS_VARIANT_LIKE_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#if !defined(__cpp_concepts) || (__cpp_concepts < 201907L)
	#error "snap/concepts/variant_like.hpp requires C++20 concepts."
#endif

#ifdef __has_include
	#if !__has_include(<concepts>)
		#error "snap/concepts/variant_like.hpp requires the <concepts> header."
	#endif
#endif

#include "snap/type_traits/remove_cvref.hpp"

#include <concepts>
#include <cstddef>
#include <utility>
#include <variant>

SNAP_BEGIN_NAMESPACE

namespace concepts
{
	namespace detail
	{
		template <class U, std::size_t... Is> constexpr bool variant_like_alternatives(std::index_sequence<Is...>)
		{
			return ((requires { typename std::variant_alternative_t<Is, U>; }) && ...);
		}

		template <class U> constexpr bool variant_like_impl()
		{
			if constexpr (!requires { std::variant_size_v<U>; }) { return false; }
			else { return variant_like_alternatives<U>(std::make_index_sequence<std::variant_size_v<U>>{}); }
		}
	} // namespace detail

	template <class T>
	concept variant_like = detail::variant_like_impl<::SNAP_NAMESPACE::remove_cvref_t<T>>();
} // namespace concepts

SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_CONCEPTS_VARIANT_LIKE_HPP
