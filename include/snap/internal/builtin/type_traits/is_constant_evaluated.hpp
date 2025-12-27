#ifndef SNP_INCLUDE_SNAP_INTERNAL_BUILTIN_TYPE_TRAITS_IS_CONSTANT_EVALUATED_HPP
#define SNP_INCLUDE_SNAP_INTERNAL_BUILTIN_TYPE_TRAITS_IS_CONSTANT_EVALUATED_HPP

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

#ifndef SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
	// Must be included first
	#include "snap/internal/abi_namespace.hpp"

	#if defined(__GNUC__) && (__GNUC__ > 9 || (__GNUC__ == 9 && __GNUC_MINOR__ >= 1)) && !defined(__clang__) && !defined(__NVCOMPILER) &&                      \
		!defined(__NVCOMPILER_LLVM__) && !defined(__CUDACC__)
		#define SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
	#endif
#endif // SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED

// Clang 9.0.0+ (non-Apple)
#ifndef SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
	#if defined(__clang__) && !defined(__apple_build_version__) && (__clang_major__ >= 9)
		#define SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
	#endif
#endif // SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED

// Apple Clang 9.0.0+
#ifndef SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
	#if defined(__clang__) && defined(__apple_build_version__) && (__clang_major__ >= 9)
		#define SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
	#endif
#endif // SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED

// Clang-CL 9.0.0+
#ifndef SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
	#if defined(__clang__) && defined(_MSC_VER) && (__clang_major__ >= 9)
		#define SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
	#endif
#endif // SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED

// DPC++ 2021.1.2+
#ifndef SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
	#if (defined(SYCL_LANGUAGE_VERSION) || defined(__INTEL_LLVM_COMPILER)) && (__INTEL_LLVM_COMPILER >= 20210102)
		#define SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
	#endif
#endif // SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED

// NVIDIA HPC 22.7+
#ifndef SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
	#if (defined(__NVCOMPILER) || defined(__NVCOMPILER_LLVM__)) && (__NVCOMPILER_MAJOR__ >= 22 && __NVCOMPILER_MINOR__ >= 7)
		#define SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
	#endif
#endif // SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED

// MSVC 19.25+ (not Clang-CL)
#ifndef SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
	#if defined(_MSC_VER) && !defined(__clang__) && (_MSC_VER >= 1925)
		#define SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
	#endif
#endif // SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED

// Final fallback: direct builtin check
#ifndef SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
	#if defined(__has_builtin)
		#if __has_builtin(__builtin_is_constant_evaluated)
			#define SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
		#endif
	#endif
#endif // SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED

SNAP_BEGIN_NAMESPACE
namespace builtin
{
	constexpr bool is_constant_evaluated() noexcept
	{
#ifdef SNAP_HAS_BUILTIN_IS_CONSTANT_EVALUATED
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

#endif // SNP_INCLUDE_SNAP_INTERNAL_BUILTIN_TYPE_TRAITS_IS_CONSTANT_EVALUATED_HPP
