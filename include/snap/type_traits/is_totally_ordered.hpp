#pragma once

#include "snap/internal/abi_namespace.hpp"
#include "snap/type_traits/is_boolean_testable.hpp"

#include <type_traits>
#include <utility> // declval

SNAP_BEGIN_NAMESPACE
template <class T, class = void> struct is_totally_ordered : std::false_type
{
};

template <class T> struct is_totally_ordered<T,
											 std::void_t<decltype(std::declval<const T &>() < std::declval<const T &>()),
														 decltype(std::declval<const T &>() > std::declval<const T &>()),
														 decltype(std::declval<const T &>() <= std::declval<const T &>()),
														 decltype(std::declval<const T &>() >= std::declval<const T &>())>>
	: std::conjunction<is_boolean_testable<decltype(std::declval<const T &>() < std::declval<const T &>())>,
					   is_boolean_testable<decltype(std::declval<const T &>() > std::declval<const T &>())>,
					   is_boolean_testable<decltype(std::declval<const T &>() <= std::declval<const T &>())>,
					   is_boolean_testable<decltype(std::declval<const T &>() >= std::declval<const T &>())>>
{
};

template <class T> inline constexpr bool is_totally_ordered_v = is_totally_ordered<T>::value;

template <class T, class U, class = void> struct is_totally_ordered_with : std::false_type
{
};

template <class T, class U> struct is_totally_ordered_with<T,
														   U,
														   std::void_t<decltype(std::declval<const T &>() < std::declval<const U &>()),
																	   decltype(std::declval<const T &>() > std::declval<const U &>()),
																	   decltype(std::declval<const T &>() <= std::declval<const U &>()),
																	   decltype(std::declval<const T &>() >= std::declval<const U &>()),
																	   decltype(std::declval<const U &>() < std::declval<const T &>()),
																	   decltype(std::declval<const U &>() > std::declval<const T &>()),
																	   decltype(std::declval<const U &>() <= std::declval<const T &>()),
																	   decltype(std::declval<const U &>() >= std::declval<const T &>())>>
	: std::conjunction<is_totally_ordered<T>,
					   is_totally_ordered<U>,
					   is_boolean_testable<decltype(std::declval<const T &>() < std::declval<const U &>())>,
					   is_boolean_testable<decltype(std::declval<const T &>() > std::declval<const U &>())>,
					   is_boolean_testable<decltype(std::declval<const T &>() <= std::declval<const U &>())>,
					   is_boolean_testable<decltype(std::declval<const T &>() >= std::declval<const U &>())>,
					   is_boolean_testable<decltype(std::declval<const U &>() < std::declval<const T &>())>,
					   is_boolean_testable<decltype(std::declval<const U &>() > std::declval<const T &>())>,
					   is_boolean_testable<decltype(std::declval<const U &>() <= std::declval<const T &>())>,
					   is_boolean_testable<decltype(std::declval<const U &>() >= std::declval<const T &>())>>
{
};

template <class T, class U> inline constexpr bool is_totally_ordered_with_v = is_totally_ordered_with<T, U>::value;
SNAP_END_NAMESPACE
