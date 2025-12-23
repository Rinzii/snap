#pragma once

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <type_traits>

SNAP_BEGIN_NAMESPACE
// is_signed_integer
template <class T> struct is_signed_integer : std::false_type
{
};

template <> struct is_signed_integer<signed char> : std::true_type
{
};

template <> struct is_signed_integer<signed short> : std::true_type
{
};

template <> struct is_signed_integer<signed int> : std::true_type
{
};

template <> struct is_signed_integer<signed long> : std::true_type
{
};

template <> struct is_signed_integer<signed long long> : std::true_type
{
};

#if defined(__SIZEOF_INT128__) && !defined(_MSC_VER)
template <> struct is_signed_integer<__int128_t> : std::true_type
{
};
#endif

template <class T> inline constexpr bool is_signed_integer_v = is_signed_integer<T>::value;

// is_unsigned_integer
template <class> struct is_unsigned_integer : std::false_type
{
};

template <> struct is_unsigned_integer<unsigned char> : std::true_type
{
};

template <> struct is_unsigned_integer<unsigned short> : std::true_type
{
};

template <> struct is_unsigned_integer<unsigned int> : std::true_type
{
};

template <> struct is_unsigned_integer<unsigned long> : std::true_type
{
};

template <> struct is_unsigned_integer<unsigned long long> : std::true_type
{
};

#if defined(__SIZEOF_INT128__) && !defined(_MSC_VER)
template <> struct is_unsigned_integer<__uint128_t> : std::true_type
{
};
#endif

template <class T> inline constexpr bool is_unsigned_integer_v = is_unsigned_integer<T>::value;

// is_integer
template <class T> struct is_integer : std::integral_constant<bool, is_signed_integer<T>::value || is_unsigned_integer<T>::value>
{
};

template <class T> inline constexpr bool is_integer_v = is_integer<T>::value;

SNAP_END_NAMESPACE
