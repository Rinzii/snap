#include "snap/utility/unreachable.hpp"

SNAP_BEGIN_NAMESPACE

[[noreturn]] void unreachable()
{
#if defined(_MSC_VER) && !defined(__clang__)
	__assume(false);
#else
	__builtin_unreachable();
#endif
}

SNAP_END_NAMESPACE
