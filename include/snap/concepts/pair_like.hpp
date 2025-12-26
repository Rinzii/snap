// snap/concepts/pair_like.hpp
#ifndef SNP_INCLUDE_SNAP_CONCEPTS_PAIR_LIKE_HPP
#define SNP_INCLUDE_SNAP_CONCEPTS_PAIR_LIKE_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#if !defined(__cpp_concepts) || (__cpp_concepts < 201907L)
	#error "snap/concepts/pair_like.hpp requires C++20 concepts."
#endif

#ifdef __has_include
	#if !__has_include(<concepts>)
		#error "snap/concepts/pair_like.hpp requires the <concepts> header."
	#endif
#endif

#include "snap/concepts/tuple_like.hpp"
#include "snap/type_traits/remove_cvref.hpp"

#include <concepts>
#include <tuple>

SNAP_BEGIN_NAMESPACE

namespace concepts
{
	template <class T>
	concept pair_like = tuple_like<T> && (std::tuple_size_v<::SNAP_NAMESPACE::remove_cvref_t<T>> == 2);
} // namespace concepts

SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_CONCEPTS_PAIR_LIKE_HPP
