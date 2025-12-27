#ifndef SNP_INCLUDE_SNAP_INTERNAL_BUILTIN_BIT_BIT_CAST_HPP
#define SNP_INCLUDE_SNAP_INTERNAL_BUILTIN_BIT_BIT_CAST_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <type_traits>

/// SNAP_HAS_BUILTIN_BIT_CAST
/// Defined if the compiler has __builtin_bit_cast
///
/// Compilers with Support:
/// - GCC 11.1+
/// - Clang 9.0.0+
/// - Apple Clang 9.0.0+
/// - Clang-CL 9.0.0+
/// - DPC++ 2021.1.2+
/// - NVIDIA HPC 22.7+
/// - MSVC 19.27+

// GCC 11.1+ (no Clang/NVHPC/CUDA frontends)
#ifndef SNAP_HAS_BUILTIN_BIT_CAST
	#if defined(__GNUC__) && (__GNUC__ > 11 || (__GNUC__ == 11 && __GNUC_MINOR__ >= 1)) && !defined(__clang__) && !defined(__NVCOMPILER) &&                    \
		!defined(__NVCOMPILER_LLVM__) && !defined(__CUDACC__)
		#define SNAP_HAS_BUILTIN_BIT_CAST
	#endif
#endif // SNAP_HAS_BUILTIN_BIT_CAST

// Clang 9.0.0+ (non-Apple)
#ifndef SNAP_HAS_BUILTIN_BIT_CAST
	#if defined(__clang__) && !defined(__apple_build_version__) && (__clang_major__ >= 9)
		#define SNAP_HAS_BUILTIN_BIT_CAST
	#endif
#endif // SNAP_HAS_BUILTIN_BIT_CAST

// Apple Clang 9.0.0+
#ifndef SNAP_HAS_BUILTIN_BIT_CAST
	#if defined(__clang__) && defined(__apple_build_version__) && (__clang_major__ >= 9)
		#define SNAP_HAS_BUILTIN_BIT_CAST
	#endif
#endif // SNAP_HAS_BUILTIN_BIT_CAST

// Clang-CL 9.0.0+
#ifndef SNAP_HAS_BUILTIN_BIT_CAST
	#if defined(__clang__) && defined(_MSC_VER) && (__clang_major__ >= 9)
		#define SNAP_HAS_BUILTIN_BIT_CAST
	#endif
#endif // SNAP_HAS_BUILTIN_BIT_CAST

// DPC++ 2021.1.2+
#ifndef SNAP_HAS_BUILTIN_BIT_CAST
	#if (defined(SYCL_LANGUAGE_VERSION) || defined(__INTEL_LLVM_COMPILER)) && (__INTEL_LLVM_COMPILER >= 20210102)
		#define SNAP_HAS_BUILTIN_BIT_CAST
	#endif
#endif // SNAP_HAS_BUILTIN_BIT_CAST

// NVIDIA HPC 22.7+
#ifndef SNAP_HAS_BUILTIN_BIT_CAST
	#if (defined(__NVCOMPILER) || defined(__NVCOMPILER_LLVM__)) && (__NVCOMPILER_MAJOR__ >= 22 && __NVCOMPILER_MINOR__ >= 7)
		#define SNAP_HAS_BUILTIN_BIT_CAST
	#endif
#endif // SNAP_HAS_BUILTIN_BIT_CAST

// MSVC 19.27+ (not Clang-CL)
#ifndef SNAP_HAS_BUILTIN_BIT_CAST
	#if defined(_MSC_VER) && !defined(__clang__) && (_MSC_VER >= 1927)
		#define SNAP_HAS_BUILTIN_BIT_CAST
	#endif
#endif // SNAP_HAS_BUILTIN_BIT_CAST

// Final fallback: direct builtin check
#ifndef SNAP_HAS_BUILTIN_BIT_CAST
	#if defined(__has_builtin)
		#if __has_builtin(__builtin_bit_cast)
			#define SNAP_HAS_BUILTIN_BIT_CAST
		#endif
	#endif
#endif // SNAP_HAS_BUILTIN_BIT_CAST

SNAP_BEGIN_NAMESPACE
namespace builtin
{
inline constexpr bool has_constexpr_bit_cast =
#ifdef SNAP_HAS_BUILTIN_BIT_CAST
		true
#else
		false
#endif
		;

	template <class To, class From>
	constexpr std::enable_if_t<has_constexpr_bit_cast && (sizeof(To) == sizeof(From)) && std::is_trivially_copyable_v<To> && std::is_trivially_copyable_v<From>,
							   To>
	bit_cast(const From& from) noexcept
	{
		return __builtin_bit_cast(To, from);
	}
} // namespace builtin
SNAP_END_NAMESPACE

#ifdef SNAP_HAS_BUILTIN_BIT_CAST
	#undef SNAP_HAS_BUILTIN_BIT_CAST
#endif

#endif // SNP_INCLUDE_SNAP_INTERNAL_BUILTIN_BIT_BIT_CAST_HPP
