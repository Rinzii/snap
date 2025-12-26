// snap/concepts/tuple_like.hpp
#ifndef SNP_INCLUDE_SNAP_CONCEPTS_TUPLE_LIKE_HPP
#define SNP_INCLUDE_SNAP_CONCEPTS_TUPLE_LIKE_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#if !defined(__cpp_concepts) || (__cpp_concepts < 201907L)
	#error "snap/concepts/tuple_like.hpp requires C++20 concepts."
#endif

#ifdef __has_include
	#if !__has_include(<concepts>)
		#error "snap/concepts/tuple_like.hpp requires the <concepts> header."
	#endif
#endif

#include "snap/type_traits/remove_cvref.hpp"

#include <concepts>
#include <cstddef>
#include <tuple>
#include <utility>

SNAP_BEGIN_NAMESPACE

namespace concepts
{
	namespace detail
	{
		template <class U, std::size_t... Is> constexpr bool tuple_like_elements(std::index_sequence<Is...>)
		{
			return ((requires(U& u) {
						typename std::tuple_element_t<Is, U>;
						std::get<Is>(u);
					}) && ...);
		}

		template <class U> constexpr bool tuple_like_impl()
		{
			if constexpr (!requires { std::tuple_size_v<U>; }) { return false; }
			else { return tuple_like_elements<U>(std::make_index_sequence<std::tuple_size_v<U>>{}); }
		}
	} // namespace detail

	template <class T>
	concept tuple_like = detail::tuple_like_impl<::SNAP_NAMESPACE::remove_cvref_t<T>>();
} // namespace concepts

SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_CONCEPTS_TUPLE_LIKE_HPP
