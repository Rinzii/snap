// snap/concepts/boolean_testable.hpp
#ifndef SNP_INCLUDE_SNAP_CONCEPTS_BOOLEAN_TESTABLE_HPP
#define SNP_INCLUDE_SNAP_CONCEPTS_BOOLEAN_TESTABLE_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#if !defined(__cpp_concepts) || (__cpp_concepts < 201907L)
	#error "snap/concepts/boolean_testable.hpp requires C++20 concepts."
#endif

#ifdef __has_include
	#if !__has_include(<concepts>)
		#error "snap/concepts/boolean_testable.hpp requires the <concepts> header."
	#endif
#endif

#include <concepts>
#include <utility>

SNAP_BEGIN_NAMESPACE

namespace concepts
{
	template <class T>
	concept boolean_testable = std::convertible_to<T, bool> && requires(T&& t) {
		{ !std::forward<T>(t) } -> std::convertible_to<bool>;
	};
} // namespace concepts

SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_CONCEPTS_BOOLEAN_TESTABLE_HPP
