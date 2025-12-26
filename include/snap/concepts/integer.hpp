// snap/concepts/integer.hpp
#pragma once

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#if !defined(__cpp_concepts) || (__cpp_concepts < 201907L)
	#error "snap/concepts/integer.hpp requires C++20 concepts."
#endif

#ifdef __has_include
	#if !__has_include(<concepts>)
		#error "snap/concepts/integer.hpp requires the <concepts> header."
	#endif
#endif

#include "snap/type_traits/remove_cvref.hpp"

#include <concepts>

SNAP_BEGIN_NAMESPACE

namespace concepts
{
	template <class T>
	concept integer = std::integral<T> && (!std::same_as<::SNAP_NAMESPACE::remove_cvref_t<T>, bool>);
} // namespace concepts

SNAP_END_NAMESPACE
