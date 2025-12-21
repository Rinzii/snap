#pragma once

#include "snap/internal/abi_namespace.hpp"

#if defined(__has_builtin)
	#if __has_builtin(__builtin_is_constant_evaluated)
		#define SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
	#endif
#endif

// GCC/Clang 9 and later has __builtin_is_constant_evaluated
#if (defined(__GNUC__) && (__GNUC__ >= 9)) || (defined(__clang__) && (__clang_major__ >= 9))
	#define SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
#endif

// Visual Studio 2019 and later supports __builtin_is_constant_evaluated
#if defined(_MSC_FULL_VER) && (_MSC_FULL_VER >= 192528326)
	#define SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
#endif

SNAP_BEGIN_NAMESPACE
constexpr bool is_constant_evaluated() noexcept
	{
#if defined(SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED)
		return __builtin_is_constant_evaluated();
#else
		return false;
#endif
	}
SNAP_END_NAMESPACE

#undef SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
