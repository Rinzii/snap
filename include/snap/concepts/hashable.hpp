// snap/concepts/hashable.hpp
#pragma once

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#if !defined(__cpp_concepts) || (__cpp_concepts < 201907L)
	#error "snap/concepts/hashable.hpp requires C++20 concepts."
#endif

#ifdef __has_include
	#if !__has_include(<concepts>)
		#error "snap/concepts/hashable.hpp requires the <concepts> header."
	#endif
#endif

#include "snap/type_traits/remove_cvref.hpp"

#include <concepts>
#include <cstddef>
#include <functional>

SNAP_BEGIN_NAMESPACE

namespace concepts
{
	template <class T>
	concept hashable = requires(const ::SNAP_NAMESPACE::remove_cvref_t<T>& v) {
		{ std::hash<::SNAP_NAMESPACE::remove_cvref_t<T>>{}(v) } -> std::convertible_to<std::size_t>;
	};
} // namespace concepts

SNAP_END_NAMESPACE
