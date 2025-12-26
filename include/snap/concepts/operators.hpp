// snap/concepts/operators.hpp
#ifndef SNP_INCLUDE_SNAP_CONCEPTS_OPERATORS_HPP
#define SNP_INCLUDE_SNAP_CONCEPTS_OPERATORS_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#if !defined(__cpp_concepts) || (__cpp_concepts < 201907L)
	#error "snap/concepts/operators.hpp requires C++20 concepts."
#endif

SNAP_BEGIN_NAMESPACE

namespace concepts
{
	template <class T, class U = T>
	concept addable = requires(T t, U u) { t + u; };

	template <class T, class U = T>
	concept subtractable = requires(T t, U u) { t - u; };

	template <class T, class U = T>
	concept multipliable = requires(T t, U u) { t * u; };

	template <class T, class U = T>
	concept divisible = requires(T t, U u) { t / u; };
} // namespace concepts

SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_CONCEPTS_OPERATORS_HPP
