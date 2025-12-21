#pragma once

#include "snap/internal/abi_namespace.hpp"

#include "snap/meta/detector.hpp"

#include <type_traits>
#include <utility>

SNAP_BEGIN_NAMESPACE
template <class T> using pointer_member_t			   = typename T::pointer;
	template <class T> using element_type_member_t		   = typename T::element_type;
	template <class T> using pointer_traits_element_type_t = typename std::pointer_traits<T>::element_type;

	template <class T, class Enable = void> struct pointer_of
	{
	};

	template <class T> struct pointer_of<T, std::enable_if_t<is_detected_v<pointer_member_t, T>, void>>
	{
		using type = typename T::pointer;
	};

	template <class T> struct pointer_of<T, std::enable_if_t<!is_detected_v<pointer_member_t, T> && is_detected_v<element_type_member_t, T>, void>>
	{
		using type = typename T::element_type *;
	};

	template <class T> struct pointer_of<
		T,
		std::enable_if_t<!is_detected_v<pointer_member_t, T> && !is_detected_v<element_type_member_t, T> && is_detected_v<pointer_traits_element_type_t, T>,
						 void>>
	{
		using type = typename std::pointer_traits<T>::element_type *;
	};

	template <class T> using pointer_of_t = typename pointer_of<T>::type;

	template <class T, class U> using pointer_of_or_t = detected_or_t<U, pointer_of_t, T>;
SNAP_END_NAMESPACE
