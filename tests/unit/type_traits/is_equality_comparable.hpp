#pragma once

#include "snap/internal/abi_namespace.hpp"

#include "snap/type_traits/is_boolean_testable.hpp"

#include <type_traits>
#include <utility> // declval

SNAP_BEGIN_NAMESPACE
template <class T, class = void> struct is_equality_comparable : std::false_type
	{
	};
	template <class T> struct is_equality_comparable<
		T,
		std::void_t<decltype(std::declval<const T &>() == std::declval<const T &>()), decltype(std::declval<const T &>() != std::declval<const T &>())>>
		: std::conjunction<is_boolean_testable<decltype(std::declval<const T &>() == std::declval<const T &>())>,
						   is_boolean_testable<decltype(std::declval<const T &>() != std::declval<const T &>())>>
	{
	};

	template <class T> inline constexpr bool is_equality_comparable_v = is_equality_comparable<T>::value;

	template <class T, class U, class = void> struct is_equality_comparable_with : std::false_type
	{
	};
	template <class T, class U> struct is_equality_comparable_with<T,
																   U,
																   std::void_t<decltype(std::declval<const T &>() == std::declval<const U &>()),
																			   decltype(std::declval<const T &>() != std::declval<const U &>()),
																			   decltype(std::declval<const U &>() == std::declval<const T &>()),
																			   decltype(std::declval<const U &>() != std::declval<const T &>())>>
		: std::conjunction<is_equality_comparable<T>,
						   is_equality_comparable<U>,
						   is_boolean_testable<decltype(std::declval<const T &>() == std::declval<const U &>())>,
						   is_boolean_testable<decltype(std::declval<const T &>() != std::declval<const U &>())>,
						   is_boolean_testable<decltype(std::declval<const U &>() == std::declval<const T &>())>,
						   is_boolean_testable<decltype(std::declval<const U &>() != std::declval<const T &>())>>
	{
	};

	template <class T, class U> inline constexpr bool is_equality_comparable_with_v = is_equality_comparable_with<T, U>::value;

SNAP_END_NAMESPACE
