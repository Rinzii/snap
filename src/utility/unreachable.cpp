#include "snap/utility/unreachable.hpp"

#include <cstdlib>

#if defined(_MSC_VER) && !defined(__clang__)
	#include <intrin.h>
#endif

SNAP_BEGIN_NAMESPACE

[[noreturn]] inline void unreachable() noexcept
{
	#if defined(_MSC_VER) && !defined(__clang__)
	__fastfail(0u);
	std::abort();
	__assume(0);
	#else
	__builtin_trap();
	__builtin_unreachable();
	#endif
}

SNAP_END_NAMESPACE
