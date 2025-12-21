#pragma once

#include "snap/internal/abi_namespace.hpp"

#include <functional>

SNAP_BEGIN_NAMESPACE
struct identity
{
	template <class T> [[nodiscard]] constexpr T&& operator()(T&& t) const noexcept { return std::forward<T>(t); }
	using is_transparent = void;
};

template <class T> struct is_identity : std::is_same<typename std::remove_cv<T>::type, ::snap::identity>
{
};

template <class U> struct is_identity<std::reference_wrapper<U>> : std::is_same<typename std::remove_cv<U>::type, ::snap::identity>
{
};

template <class T> inline constexpr bool is_identity_v = is_identity<T>::value;
SNAP_END_NAMESPACE
