#pragma once

#include "snap/internal/abi_namespace.hpp"
#include "snap/memory/pointer_of.hpp"
#include "snap/meta/detector.hpp"

#include <type_traits>
#include <utility>

SNAP_BEGIN_NAMESPACE
template <class Smart> using reset_expr_t = decltype(std::declval<Smart &>().reset());

template <class Smart> struct is_resettable_smart_pointer : is_detected<reset_expr_t, Smart>
{
};

template <class Smart> inline constexpr bool is_resettable_smart_pointer_v = is_resettable_smart_pointer<Smart>::value;

template <class Smart> using release_expr_t = decltype(std::declval<Smart &>().release());

template <class Smart> struct is_releasable_smart_pointer : is_detected<release_expr_t, Smart>
{
};

template <class Smart> inline constexpr bool is_releasable_smart_pointer_v = is_releasable_smart_pointer<Smart>::value;

template <class Smart, class Pointer, class... Args> using reset_with_args_expr_t =
	decltype(std::declval<Smart &>().reset(static_cast<pointer_of_or_t<Smart, Pointer>>(std::declval<Pointer>()), std::declval<Args>()...));

template <class Smart, class Pointer, class... Args> struct is_resettable_smart_pointer_with_args : is_detected<reset_with_args_expr_t, Smart, Pointer, Args...>
{
};

template <class Smart, class Pointer, class... Args> inline constexpr bool is_resettable_smart_pointer_with_args_v =
	is_resettable_smart_pointer_with_args<Smart, Pointer, Args...>::value;

SNAP_END_NAMESPACE
