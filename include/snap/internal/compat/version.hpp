#pragma once

#include "snap/internal/compat/std.hpp"

#if defined(__has_include) && __has_include(<version>) // If we have the version header just use that instead.
	#include <version>
#elif defined(_MSC_VER) && defined(_WIN32) && defined(__has_include) && __has_include(<yvals_core.h>) // MSVC actually backports these macros in yvals_core.h
	#include <yvals_core.h>

// General feature testing backport. Disable-able through macro.
#elif !defined(SNAP_DISABLE_FEATURE_TEST_BACKPORTS)

/// C++14

	#if !defined(__cpp_lib_chrono_udls)
		#if SNAP_LANG_AT_LEAST(201402)
			#define __cpp_lib_chrono_udls 201304L
		#endif
	#endif

	#if !defined(__cpp_lib_complex_udls)
		#if SNAP_LANG_AT_LEAST(201402) && ((defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 1)) || (defined(_GLIBCXX_HOSTED) && _GLIBCXX_HOSTED) ||            \
										   defined(_LIBCPP_VERSION) || defined(_CPPLIB_VER) || defined(_YVALS))
			#define __cpp_lib_complex_udls 201309L
		#endif
	#endif

	#if !defined(__cpp_lib_exchange_function)
		#if SNAP_LANG_AT_LEAST(201402)
			#define __cpp_lib_exchange_function 201304L
		#endif
	#endif

	#if !defined(__cpp_lib_generic_associative_lookup)
		#if SNAP_LANG_AT_LEAST(201402) && ((defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 1)) || (defined(_GLIBCXX_HOSTED) && _GLIBCXX_HOSTED) ||            \
										   defined(_LIBCPP_VERSION) || defined(_CPPLIB_VER) || defined(_YVALS))
			#define __cpp_lib_generic_associative_lookup 201304L
		#endif
	#endif

	#if !defined(__cpp_lib_integer_sequence)
		#if SNAP_LANG_AT_LEAST(201402)
			#define __cpp_lib_integer_sequence 201304L
		#endif
	#endif

	#if !defined(__cpp_lib_integral_constant_callable)
		#if SNAP_LANG_AT_LEAST(201402)
			#define __cpp_lib_integral_constant_callable 201304L
		#endif
	#endif

	#if !defined(__cpp_lib_is_final)
		#if SNAP_LANG_AT_LEAST(201402)
			#define __cpp_lib_is_final 201402L
		#endif
	#endif

	#if !defined(__cpp_lib_is_null_pointer)
		#if SNAP_LANG_AT_LEAST(201103)
			#define __cpp_lib_is_null_pointer 201309L
		#endif
	#endif

	#if !defined(__cpp_lib_make_reverse_iterator)
		#if SNAP_LANG_AT_LEAST(201402)
			#define __cpp_lib_make_reverse_iterator 201402L
		#endif
	#endif

	#if !defined(__cpp_lib_make_unique)
		#if SNAP_LANG_AT_LEAST(201402) && ((defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 1)) || (defined(_GLIBCXX_HOSTED) && _GLIBCXX_HOSTED) ||            \
										   defined(_LIBCPP_VERSION) || defined(_CPPLIB_VER) || defined(_YVALS))
			#define __cpp_lib_make_unique 201304L
		#endif
	#endif

	#if !defined(__cpp_lib_null_iterators)
		#if SNAP_LANG_AT_LEAST(201402)
			#define __cpp_lib_null_iterators 201304L
		#endif
	#endif

	#if !defined(__cpp_lib_quoted_string_io)
		#if ((defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 1)) || (defined(_GLIBCXX_HOSTED) && _GLIBCXX_HOSTED) || defined(_LIBCPP_VERSION) ||              \
			 defined(_CPPLIB_VER) || defined(_YVALS))
			#define __cpp_lib_quoted_string_io 201304L
		#endif
	#endif

	#if !defined(__cpp_lib_result_of_sfinae)
		#if SNAP_LANG_AT_LEAST(201103)
			#define __cpp_lib_result_of_sfinae 201210L
		#endif
	#endif

	#if !defined(__cpp_lib_robust_nonmodifying_seq_ops)
		#if SNAP_LANG_AT_LEAST(201402)
			#define __cpp_lib_robust_nonmodifying_seq_ops 201304L
		#endif
	#endif

	#if !defined(__cpp_lib_shared_timed_mutex)
		#if defined(_LIBCPP_HAS_THREAD_API_PTHREAD) || defined(_GLIBCXX_HAS_GTHREADS) || defined(_CPPLIB_VER) || defined(_YVALS)
			#define __cpp_lib_shared_timed_mutex 201402L
		#endif
	#endif

	#if !defined(__cpp_lib_string_udls)
		#if SNAP_LANG_AT_LEAST(201402) && ((defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 1)) || (defined(_GLIBCXX_HOSTED) && _GLIBCXX_HOSTED) ||            \
										   defined(_LIBCPP_VERSION) || defined(_CPPLIB_VER) || defined(_YVALS))
			#define __cpp_lib_string_udls 201304L
		#endif
	#endif

	#if !defined(__cpp_lib_transformation_trait_aliases)
		#if SNAP_LANG_AT_LEAST(201402)
			#define __cpp_lib_transformation_trait_aliases 201304L
		#endif
	#endif

	#if !defined(__cpp_lib_transparent_operators)
		#if SNAP_LANG_AT_LEAST(201402)
			#define __cpp_lib_transparent_operators 201510L
		#endif
	#endif

	#if !defined(__cpp_lib_tuple_element_t)
		#if SNAP_LANG_AT_LEAST(201402)
			#define __cpp_lib_tuple_element_t 201402L
		#endif
	#endif

	#if !defined(__cpp_lib_tuples_by_type)
		#if SNAP_LANG_AT_LEAST(201402)
			#define __cpp_lib_tuples_by_type 201304L
		#endif
	#endif

/// C++17

	#if !defined(__cpp_lib_addressof_constexpr)
		#if SNAP_LANG_AT_LEAST(201703)
			#define __cpp_lib_addressof_constexpr 201603L
		#endif
	#endif

	#if !defined(__cpp_lib_allocator_traits_is_always_equal)
		#if SNAP_LANG_AT_LEAST(201103)
			#define __cpp_lib_allocator_traits_is_always_equal 201411L
		#endif
	#endif

	#if !defined(__cpp_lib_any)
		#if SNAP_LANG_AT_LEAST(201703) && (defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 1))
			#define __cpp_lib_any 201606L
		#endif
	#endif

	#if !defined(__cpp_lib_apply)
		#if SNAP_LANG_AT_LEAST(201703)
			#define __cpp_lib_apply 201603L
		#endif
	#endif

	#if !defined(__cpp_lib_array_constexpr)
		#if SNAP_LANG_AT_LEAST(201703)
			#define __cpp_lib_array_constexpr 201603L
		#endif
	#endif

	#if !defined(__cpp_lib_as_const)
		#if SNAP_LANG_AT_LEAST(201703)
			#define __cpp_lib_as_const 201510L
		#endif
	#endif

	#if !defined(__cpp_lib_atomic_is_always_lock_free)
		#if SNAP_LANG_AT_LEAST(201703)
			#define __cpp_lib_atomic_is_always_lock_free 201603L
		#endif
	#endif

	#if !defined(__cpp_lib_bool_constant)
		#if SNAP_LANG_AT_LEAST(201703)
			#define __cpp_lib_bool_constant 201505L
		#endif
	#endif

	#if !defined(__cpp_lib_boyer_moore_searcher)
		#if SNAP_LANG_AT_LEAST(201703) && ((defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 1)) || (defined(_GLIBCXX_HOSTED) && _GLIBCXX_HOSTED) ||            \
										   defined(_LIBCPP_VERSION) || defined(_CPPLIB_VER) || defined(_YVALS))
			#define __cpp_lib_boyer_moore_searcher 201603L
		#endif
	#endif

	#if !defined(__cpp_lib_byte)
		#if SNAP_LANG_AT_LEAST(201703)
			#define __cpp_lib_byte 201603L
		#endif
	#endif

	#if !defined(__cpp_lib_chrono)
		#if SNAP_LANG_AT_LEAST(201703) && ((defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 1)) || (defined(_GLIBCXX_HOSTED) && _GLIBCXX_HOSTED) ||            \
										   defined(_LIBCPP_VERSION) || defined(_CPPLIB_VER) || defined(_YVALS))
			#define __cpp_lib_chrono 201611L
		#endif
	#endif

	#if !defined(__cpp_lib_clamp)
		#if SNAP_LANG_AT_LEAST(201703)
			#define __cpp_lib_clamp 201603L
		#endif
	#endif

	#if !defined(__cpp_lib_enable_shared_from_this)
		#if SNAP_LANG_AT_LEAST(201703) && ((defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 1)) || (defined(_GLIBCXX_HOSTED) && _GLIBCXX_HOSTED) ||            \
										   defined(_LIBCPP_VERSION) || defined(_CPPLIB_VER) || defined(_YVALS))
			#define __cpp_lib_enable_shared_from_this 201603L
		#endif
	#endif

	#if !defined(__cpp_lib_filesystem)
		#if SNAP_LANG_AT_LEAST(201703) && (defined(_LIBCPP_VERSION) || defined(__GLIBCXX__) || defined(_CPPLIB_VER) || defined(_YVALS))
			#define __cpp_lib_filesystem 201703L
		#endif
	#endif

	#if !defined(__cpp_lib_gcd_lcm)
		#if SNAP_LANG_AT_LEAST(201703)
			#define __cpp_lib_gcd_lcm 201606L
		#endif
	#endif

	#if !defined(__cpp_lib_hardware_interference_size)
		#if defined(__GCC_DESTRUCTIVE_SIZE) || defined(__GCC_CONSTRUCTIVE_SIZE)
			#define __cpp_lib_hardware_interference_size 201703L
		#endif
	#endif

	#if !defined(__cpp_lib_has_unique_object_representations)
		#if SNAP_LANG_AT_LEAST(201703)
			#define __cpp_lib_has_unique_object_representations 201606L
		#endif
	#endif

	#if !defined(__cpp_lib_hypot)
		#if SNAP_LANG_AT_LEAST(201703) && ((defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 1)) || (defined(_GLIBCXX_HOSTED) && _GLIBCXX_HOSTED) ||            \
										   defined(_LIBCPP_VERSION) || defined(_CPPLIB_VER) || defined(_YVALS))
			#define __cpp_lib_hypot 201603L
		#endif
	#endif

	#if !defined(__cpp_lib_incomplete_container_elements)
		#if SNAP_LANG_AT_LEAST(201703) && ((defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 1)) || (defined(_GLIBCXX_HOSTED) && _GLIBCXX_HOSTED) ||            \
										   defined(_LIBCPP_VERSION) || defined(_CPPLIB_VER) || defined(_YVALS))
			#define __cpp_lib_incomplete_container_elements 201505L
		#endif
	#endif

	#if !defined(__cpp_lib_invoke)
		#if SNAP_LANG_AT_LEAST(201703)
			#define __cpp_lib_invoke 201411L
		#endif
	#endif

	#if !defined(__cpp_lib_is_aggregate)
		#if SNAP_LANG_AT_LEAST(201703)
			#define __cpp_lib_is_aggregate 201703L
		#endif
	#endif

	#if !defined(__cpp_lib_is_invocable)
		#if SNAP_LANG_AT_LEAST(201703)
			#define __cpp_lib_is_invocable 201703L
		#endif
	#endif

	#if !defined(__cpp_lib_is_swappable)
		#if SNAP_LANG_AT_LEAST(201703)
			#define __cpp_lib_is_swappable 201603L
		#endif
	#endif

	#if !defined(__cpp_lib_launder)
		#if SNAP_LANG_AT_LEAST(201703)
			#define __cpp_lib_launder 201606L
		#endif
	#endif

	#if !defined(__cpp_lib_logical_traits)
		#if SNAP_LANG_AT_LEAST(201703)
			#define __cpp_lib_logical_traits 201510L
		#endif
	#endif

	#if !defined(__cpp_lib_make_from_tuple)
		#if SNAP_LANG_AT_LEAST(201703)
			#define __cpp_lib_make_from_tuple 201606L
		#endif
	#endif

	#if !defined(__cpp_lib_map_try_emplace)
		#if SNAP_LANG_AT_LEAST(201703) && ((defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 1)) || (defined(_GLIBCXX_HOSTED) && _GLIBCXX_HOSTED) ||            \
										   defined(_LIBCPP_VERSION) || defined(_CPPLIB_VER) || defined(_YVALS))
			#define __cpp_lib_map_try_emplace 201411L
		#endif
	#endif

	#if !defined(__cpp_lib_memory_resource)
		#if defined(_LIBCPP_VERSION) || defined(__GLIBCXX__) || defined(_CPPLIB_VER) || defined(_YVALS)
			#define __cpp_lib_memory_resource 201603L
		#endif
	#endif

	#if !defined(__cpp_lib_node_extract)
		#if SNAP_LANG_AT_LEAST(201703) && ((defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 1)) || (defined(_GLIBCXX_HOSTED) && _GLIBCXX_HOSTED) ||            \
										   defined(_LIBCPP_VERSION) || defined(_CPPLIB_VER) || defined(_YVALS))
			#define __cpp_lib_node_extract 201606L
		#endif
	#endif

	#if !defined(__cpp_lib_nonmember_container_access)
		#if SNAP_LANG_AT_LEAST(201703)
			#define __cpp_lib_nonmember_container_access 201411L
		#endif
	#endif

	#if !defined(__cpp_lib_not_fn)
		#if SNAP_LANG_AT_LEAST(201703)
			#define __cpp_lib_not_fn 201603L
		#endif
	#endif

	#if !defined(__cpp_lib_optional)
		#if SNAP_LANG_AT_LEAST(201703) && defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 1)
			#define __cpp_lib_optional 201606L
		#endif
	#endif

	#if !defined(__cpp_lib_raw_memory_algorithms)
		#if SNAP_LANG_AT_LEAST(201703)
			#define __cpp_lib_raw_memory_algorithms 201606L
		#endif
	#endif

	#if !defined(__cpp_lib_sample)
		#if SNAP_LANG_AT_LEAST(201703)
			#define __cpp_lib_sample 201603L
		#endif
	#endif

	#if !defined(__cpp_lib_scoped_lock)
		#if SNAP_LANG_AT_LEAST(201703)
			#define __cpp_lib_scoped_lock 201703L
		#endif
	#endif

	#if !defined(__cpp_lib_shared_mutex)
		#if defined(_LIBCPP_HAS_THREAD_API_PTHREAD) || defined(_GLIBCXX_HAS_GTHREADS) || defined(_CPPLIB_VER) || defined(_YVALS)
			#define __cpp_lib_shared_mutex 201505L
		#endif
	#endif

	#if !defined(__cpp_lib_shared_ptr_arrays)
		#if SNAP_LANG_AT_LEAST(201703) && ((defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 1)) || (defined(_GLIBCXX_HOSTED) && _GLIBCXX_HOSTED) ||            \
										   defined(_LIBCPP_VERSION) || defined(_CPPLIB_VER) || defined(_YVALS))
			#define __cpp_lib_shared_ptr_arrays 201611L
		#endif
	#endif

	#if !defined(__cpp_lib_shared_ptr_weak_type)
		#if SNAP_LANG_AT_LEAST(201703) && ((defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 1)) || (defined(_GLIBCXX_HOSTED) && _GLIBCXX_HOSTED) ||            \
										   defined(_LIBCPP_VERSION) || defined(_CPPLIB_VER) || defined(_YVALS))
			#define __cpp_lib_shared_ptr_weak_type 201606L
		#endif
	#endif

	#if !defined(__cpp_lib_string_view)
		#if SNAP_LANG_AT_LEAST(201703) && defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 1)
			#define __cpp_lib_string_view 201606L
		#endif
	#endif

	#if !defined(__cpp_lib_type_trait_variable_templates)
		#if SNAP_LANG_AT_LEAST(201703)
			#define __cpp_lib_type_trait_variable_templates 201510L
		#endif
	#endif

	#if !defined(__cpp_lib_uncaught_exceptions)
		#if SNAP_LANG_AT_LEAST(201703)
			#define __cpp_lib_uncaught_exceptions 201411L
		#endif
	#endif

	#if !defined(__cpp_lib_unordered_map_try_emplace)
		#if SNAP_LANG_AT_LEAST(201703) && ((defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 1)) || (defined(_GLIBCXX_HOSTED) && _GLIBCXX_HOSTED) ||            \
										   defined(_LIBCPP_VERSION) || defined(_CPPLIB_VER) || defined(_YVALS))
			#define __cpp_lib_unordered_map_try_emplace 201411L
		#endif
	#endif

	#if !defined(__cpp_lib_variant)
		#if SNAP_LANG_AT_LEAST(201703) && defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 1)
			#define __cpp_lib_variant 201606L
		#endif
	#endif

	#if !defined(__cpp_lib_void_t)
		#if SNAP_LANG_AT_LEAST(201103)
			#define __cpp_lib_void_t 201411L
		#endif
	#endif

#endif // !SNAP_DISABLE_FEATURE_TEST_BACKPORTS
