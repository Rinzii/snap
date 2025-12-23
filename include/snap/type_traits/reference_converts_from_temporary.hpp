#pragma once

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/internal/builtin/type_traits/reference_converts_from_temporary.hpp"

#include <type_traits>

SNAP_BEGIN_NAMESPACE

namespace internal
{
	template <class To, class From> inline constexpr bool ref_compatible_v =
		std::is_object_v<To> && std::is_object_v<From> ? std::is_convertible_v<std::add_pointer_t<From>, std::add_pointer_t<To>> : std::is_same_v<To, From>;

	template <class T, class V> inline constexpr bool direct_ref_bind_no_new_temp_v = []
	{
		if constexpr (!std::is_reference_v<T> || !std::is_reference_v<V>) { return false; }
		else
		{
			using TR = std::remove_reference_t<T>;
			using VR = std::remove_reference_t<V>;

			if constexpr (std::is_lvalue_reference_v<T>)
			{
				if constexpr (std::is_lvalue_reference_v<V>) { return ref_compatible_v<TR, VR>; }
				else { return (std::is_const_v<TR> && !std::is_volatile_v<TR>) && ref_compatible_v<TR, VR>; }
			}
			else
			{
				if constexpr (std::is_rvalue_reference_v<V>) { return ref_compatible_v<TR, VR>; }
				else { return false; }
			}
		}
	}();

	template <class T, class V> inline constexpr bool convertible_via_reference_no_new_temp_v = []
	{
		if constexpr (!std::is_reference_v<T>) { return false; }
		else
		{
			using TR = std::remove_reference_t<T>;

			if constexpr (std::is_rvalue_reference_v<T>) { return std::is_convertible_v<V, std::add_rvalue_reference_t<TR>>; }
			else { return std::is_convertible_v<V, std::add_lvalue_reference_t<TR>>; }
		}
	}();

	template <class T, class V> inline constexpr bool scalar_conversion_must_create_temp_v = []
	{
		if constexpr (!std::is_reference_v<T>) { return false; }
		else
		{
			using TR  = std::remove_reference_t<T>;
			using VR  = std::remove_reference_t<V>;
			using TR0 = std::remove_cv_t<TR>;
			using VR0 = std::remove_cv_t<VR>;

			if constexpr (std::is_scalar_v<TR0> && std::is_scalar_v<VR0> && !std::is_same_v<TR0, VR0>) { return std::is_convertible_v<V, T>; }
			else { return false; }
		}
	}();

	template <class T, class U> inline constexpr bool reference_converts_from_temporary_fallback_v = []
	{
		using V = builtin::adjust_ref_binding_source_t<U>;

		if constexpr (!std::is_reference_v<T>) { return false; }
		else if constexpr (!std::is_convertible_v<V, T>) { return false; }
		else if constexpr (std::is_lvalue_reference_v<T> && !(std::is_const_v<std::remove_reference_t<T>> && !std::is_volatile_v<std::remove_reference_t<T>>))
		{
			return false;
		}
		else if constexpr (!std::is_reference_v<U> && !std::is_function_v<U>) { return true; }
		else if constexpr (direct_ref_bind_no_new_temp_v<T, V>) { return false; }
		else if constexpr (scalar_conversion_must_create_temp_v<T, V>) { return true; }
		else if constexpr (convertible_via_reference_no_new_temp_v<T, V>) { return false; }
		else { return true; }
	}();

	template <class T, class U, bool = builtin::has_any_reference_temporary_builtin> struct reference_converts_from_temporary_dispatch
		: std::bool_constant<reference_converts_from_temporary_fallback_v<T, U>>
	{
	};

	template <class T, class U> struct reference_converts_from_temporary_dispatch<T, U, true>
		: std::bool_constant<builtin::reference_converts_from_temporary_v<T, U>>
	{
	};
} // namespace internal

template <class T, class U> struct reference_converts_from_temporary
#if defined(__cpp_lib_reference_from_temporary) && (__cpp_lib_reference_from_temporary >= 202202L)
	: std::bool_constant<std::reference_converts_from_temporary_v<T, U>>
#else
	: std::bool_constant<std::is_reference_v<T> && internal::reference_converts_from_temporary_dispatch<T, U>::value>
#endif
{
};

template <class T, class U> inline constexpr bool reference_converts_from_temporary_v = reference_converts_from_temporary<T, U>::value;

SNAP_END_NAMESPACE
