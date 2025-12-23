#pragma once

/// SNAP_HAS_BUILTIN_BIT_CAST
/// Defined if the compiler has __builtin_bit_cast
///
/// Compilers with Support:
/// - GCC 9.1+
/// - Clang 9.0.0+
/// - Apple Clang 9.0.0+
/// - Clang-CL 9.0.0+
/// - DPC++ 2021.1.2+
/// - NVIDIA HPC 22.7+
/// - MSVC 19.25+

#if !defined(SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED)
	// Must be included first
	#include "snap/internal/abi_namespace.hpp"

	#if defined(__GNUC__) && (__GNUC__ > 9 || (__GNUC__ == 9 && __GNUC_MINOR__ >= 1)) && !defined(__clang__) && !defined(__NVCOMPILER) &&                      \
		!defined(__NVCOMPILER_LLVM__) && !defined(__CUDACC__)
		#define SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
	#endif
#endif

// Clang 9.0.0+ (non-Apple)
#if !defined(SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED)
	#if defined(__clang__) && !defined(__apple_build_version__) && (__clang_major__ >= 9)
		#define SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
	#endif
#endif

// Apple Clang 9.0.0+
#if !defined(SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED)
	#if defined(__clang__) && defined(__apple_build_version__) && (__clang_major__ >= 9)
		#define SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
	#endif
#endif

// Clang-CL 9.0.0+
#if !defined(SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED)
	#if defined(__clang__) && defined(_MSC_VER) && (__clang_major__ >= 9)
		#define SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
	#endif
#endif

// DPC++ 2021.1.2+
#if !defined(SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED)
	#if (defined(SYCL_LANGUAGE_VERSION) || defined(__INTEL_LLVM_COMPILER)) && (__INTEL_LLVM_COMPILER >= 20210102)
		#define SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
	#endif
#endif

// NVIDIA HPC 22.7+
#if !defined(SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED)
	#if (defined(__NVCOMPILER) || defined(__NVCOMPILER_LLVM__)) && (__NVCOMPILER_MAJOR__ >= 22 && __NVCOMPILER_MINOR__ >= 7)
		#define SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
	#endif
#endif

// MSVC 19.25+ (not Clang-CL)
#if !defined(SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED)
	#if defined(_MSC_VER) && !defined(__clang__) && (_MSC_VER >= 1925)
		#define SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
	#endif
#endif

// Final fallback: direct builtin check
#if !defined(SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED)
	#if defined(__has_builtin)
		#if __has_builtin(__builtin_is_constant_evaluated)
			#define SNAP_HAS_BUILTIN_BIT_CAST
		#endif
	#endif
#endif

SNAP_BEGIN_NAMESPACE
namespace builtin
{
	constexpr bool is_constant_evaluated() noexcept
	{
#if defined(SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED)
		return __builtin_is_constant_evaluated();
#else
		return false;
#endif
	}
} // namespace builtin
SNAP_END_NAMESPACE

#ifdef SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
	#undef SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
#endif
