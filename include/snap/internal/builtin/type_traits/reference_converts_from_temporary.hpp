#ifndef SNP_INCLUDE_SNAP_INTERNAL_BUILTIN_TYPE_TRAITS_REFERENCE_CONVERTS_FROM_TEMPORARY_HPP
#define SNP_INCLUDE_SNAP_INTERNAL_BUILTIN_TYPE_TRAITS_REFERENCE_CONVERTS_FROM_TEMPORARY_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <type_traits>

#if !defined(INTERNAL_SNAP_HAS_BUILTIN_REFERENCE_CONVERTS_FROM_TEMPORARY)
	#if defined(__NVCOMPILER) || defined(__NVCOMPILER_LLVM__)
		#define INTERNAL_SNAP_HAS_BUILTIN_REFERENCE_CONVERTS_FROM_TEMPORARY 0
	#elif defined(__clang__) && (__clang_major__ > 19 || (__clang_major__ == 19 && __clang_minor__ >= 1))
		#define INTERNAL_SNAP_HAS_BUILTIN_REFERENCE_CONVERTS_FROM_TEMPORARY 1
	#elif defined(__INTEL_LLVM_COMPILER) && (__INTEL_LLVM_COMPILER >= 20250000)
		#define INTERNAL_SNAP_HAS_BUILTIN_REFERENCE_CONVERTS_FROM_TEMPORARY 1
	#elif defined(__GNUC__) && (__GNUC__ >= 13) && !defined(__clang__) && !defined(__CUDACC__)
		#define INTERNAL_SNAP_HAS_BUILTIN_REFERENCE_CONVERTS_FROM_TEMPORARY 1
	#else
		#define INTERNAL_SNAP_HAS_BUILTIN_REFERENCE_CONVERTS_FROM_TEMPORARY 0
	#endif
#endif

#if !defined(INTERNAL_SNAP_HAS_BUILTIN_REFERENCE_BINDS_TO_TEMPORARY)
	#if defined(__NVCOMPILER) || defined(__NVCOMPILER_LLVM__)
		#define INTERNAL_SNAP_HAS_BUILTIN_REFERENCE_BINDS_TO_TEMPORARY 0
	#elif defined(__clang__) && (__clang_major__ >= 7) && !defined(__CUDACC__)
		#define INTERNAL_SNAP_HAS_BUILTIN_REFERENCE_BINDS_TO_TEMPORARY 1
	#elif defined(__INTEL_LLVM_COMPILER) && (__INTEL_LLVM_COMPILER >= 20210200)
		#define INTERNAL_SNAP_HAS_BUILTIN_REFERENCE_BINDS_TO_TEMPORARY 1
	#else
		#define INTERNAL_SNAP_HAS_BUILTIN_REFERENCE_BINDS_TO_TEMPORARY 0
	#endif
#endif

#define INTERNAL_SNAP_HAS_ANY_REFERENCE_TEMPORARY_BUILTIN                                                                                                      \
	(INTERNAL_SNAP_HAS_BUILTIN_REFERENCE_CONVERTS_FROM_TEMPORARY || INTERNAL_SNAP_HAS_BUILTIN_REFERENCE_BINDS_TO_TEMPORARY)

SNAP_BEGIN_NAMESPACE
namespace builtin
{
	template <class U, class = void> struct adjust_ref_binding_source_impl
	{
		using type = U;
	};

	template <class U> struct adjust_ref_binding_source_impl<U, std::enable_if_t<!std::is_reference_v<U> && !std::is_function_v<U>>>
	{
		using type = std::remove_cv_t<U>;
	};

	template <class U> struct adjust_ref_binding_source_impl<U, std::enable_if_t<std::is_function_v<U> && std::is_same_v<void, std::void_t<U &>>>>
	{
		using type = U &;
	};

	template <class U> struct adjust_ref_binding_source
	{
		using type = typename adjust_ref_binding_source_impl<U>::type;
	};

	template <class U> using adjust_ref_binding_source_t = typename adjust_ref_binding_source<U>::type;

	inline constexpr bool has_reference_converts_from_temporary =
#if INTERNAL_SNAP_HAS_BUILTIN_REFERENCE_CONVERTS_FROM_TEMPORARY
		true
#else
		false
#endif
		;

	inline constexpr bool has_reference_binds_to_temporary =
#if INTERNAL_SNAP_HAS_BUILTIN_REFERENCE_BINDS_TO_TEMPORARY
		true
#else
		false
#endif
		;

	inline constexpr bool has_any_reference_temporary_builtin =
#if INTERNAL_SNAP_HAS_ANY_REFERENCE_TEMPORARY_BUILTIN
		true
#else
		false
#endif
		;

	template <class T, class U, class Enable = void> struct reference_converts_from_temporary_impl : std::false_type
	{
	};

#if INTERNAL_SNAP_HAS_ANY_REFERENCE_TEMPORARY_BUILTIN
	template <class T, class U> struct reference_converts_from_temporary_impl<T, U, std::enable_if_t<std::is_reference_v<T>>>
		: std::bool_constant<
	#if INTERNAL_SNAP_HAS_BUILTIN_REFERENCE_CONVERTS_FROM_TEMPORARY
			  __reference_converts_from_temporary(T, adjust_ref_binding_source_t<U>)
	#else
			  __reference_binds_to_temporary(T, adjust_ref_binding_source_t<U>)
	#endif
			  >
	{
	};
#endif // INTERNAL_SNAP_HAS_ANY_REFERENCE_TEMPORARY_BUILTIN

	template <class T, class U> inline constexpr bool reference_converts_from_temporary_v = reference_converts_from_temporary_impl<T, U>::value;

} // namespace builtin
SNAP_END_NAMESPACE

#undef INTERNAL_SNAP_HAS_ANY_REFERENCE_TEMPORARY_BUILTIN
#undef INTERNAL_SNAP_HAS_BUILTIN_REFERENCE_BINDS_TO_TEMPORARY
#undef INTERNAL_SNAP_HAS_BUILTIN_REFERENCE_CONVERTS_FROM_TEMPORARY

#endif // SNP_INCLUDE_SNAP_INTERNAL_BUILTIN_TYPE_TRAITS_REFERENCE_CONVERTS_FROM_TEMPORARY_HPP
