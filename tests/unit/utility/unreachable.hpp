#ifndef SNP_TESTS_UNIT_UTILITY_UNREACHABLE_HPP
#define SNP_TESTS_UNIT_UTILITY_UNREACHABLE_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <type_traits>

SNAP_BEGIN_NAMESPACE
[[noreturn]] inline void unreachable()
{
	// Uses compiler specific extensions if possible.
	// Even if no extension is used, undefined behavior is still raised by
	// an empty function body and the noreturn attribute.
#if defined(_MSC_VER) && !defined(__clang__) // MSVC
	__assume(false);
#else // GCC, Clang
	__builtin_unreachable();
#endif
}

SNAP_END_NAMESPACE

#endif // SNP_TESTS_UNIT_UTILITY_UNREACHABLE_HPP
